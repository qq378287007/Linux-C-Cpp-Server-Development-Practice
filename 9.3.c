#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>

#define ISspace(x) isspace((int)(x))
#define SERVER_STRING "Server: jdbhttpd/0.1.0\r\n"

void error_die(const char *sc)
{
  perror(sc);
  exit(1);
}

int get_line(int sock, char *buf, int size)
{
  char c;
  int n;
  int i = 0;
  while (i < size - 1)
  {
    n = recv(sock, &c, 1, 0);
    if (n == 0)
      break;

    if (c == '\r')
    {
      if (recv(sock, &c, 1, 0) == 0 || c == '\n')
      {
        buf[i++] = '\n';
        break;
      }
    }

    buf[i++] = c;
  }

  /*
  char c = '\0';
  while ((i < size - 1) && (c != '\n'))
  {
    int n = recv(sock, &c, 1, 0);
    if (n > 0)
    {
      if (c == '\r')
      {
        n = recv(sock, &c, 1, MSG_PEEK);
        if ((n > 0) && (c == '\n'))
          recv(sock, &c, 1, 0);
        else
          c = '\n';
      }
      buf[i] = c;
      i++;
    }
    else
      c = '\n';
  }
*/
  buf[i] = '\0';

  return i;
}

void unimplemented(int client)
{
  char buf[1024];

  sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, SERVER_STRING);
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "Content-Type: text/html\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "</TITLE></HEAD>\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "</BODY></HTML>\r\n");
  send(client, buf, strlen(buf), 0);
}

void not_found(int client)
{
  char buf[1024];

  sprintf(buf, "HTTP/1.0 404 NOT FOUND\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, SERVER_STRING);
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "Content-Type: text/html\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "<BODY><P>The server could not fulfill\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "your request because the resource specified\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "is unavailable or nonexistent.\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "</P></BODY></HTML>\r\n");
  send(client, buf, strlen(buf), 0);
}

void headers(int client, const char *filename)
{
  char buf[1024];

  strcpy(buf, "HTTP/1.0 200 OK\r\n");
  send(client, buf, strlen(buf), 0);
  strcpy(buf, SERVER_STRING);
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "Content-Type: text/html\r\n"); // could use filename to determine file type
  send(client, buf, strlen(buf), 0);
  strcpy(buf, "\r\n");
  send(client, buf, strlen(buf), 0);
}

void cat(int client, FILE *resource)
{
  char buf[1024];

  fgets(buf, sizeof(buf), resource);
  while (!feof(resource))
  {
    send(client, buf, strlen(buf), 0);
    fgets(buf, sizeof(buf), resource);
  }
}

void serve_file(int client, const char *filename)
{
  int numchars = 1;
  char buf[1024] = "A";
  while ((numchars > 0) && strcmp("\n", buf)) // read & discard headers
    numchars = get_line(client, buf, sizeof(buf));

  FILE *resource = fopen(filename, "r");
  if (resource == NULL)
    not_found(client);
  else
  {
    headers(client, filename);
    cat(client, resource);
  }
  fclose(resource);
}

int startup(u_short *port)
{
  int httpd = socket(PF_INET, SOCK_STREAM, 0);
  if (httpd == -1)
    error_die("socket");

  struct sockaddr_in name;
  int namelen = sizeof(name);
  memset(&name, 0, namelen);
  name.sin_family = AF_INET;
  name.sin_addr.s_addr = htonl(INADDR_ANY);
  name.sin_port = htons(*port);

  if (bind(httpd, (struct sockaddr *)&name, namelen) < 0)
    error_die("bind");

  if (*port == 0) // 动态分配端口
  {
    if (getsockname(httpd, (struct sockaddr *)&name, &namelen) == -1)
      error_die("getsockname");

    *port = ntohs(name.sin_port);
  }

  if (listen(httpd, 5) < 0)
    error_die("listen");

  return httpd;
}

void bad_request(int client)
{
  char buf[1024];

  sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n");
  send(client, buf, sizeof(buf), 0);
  sprintf(buf, "Content-type: text/html\r\n");
  send(client, buf, sizeof(buf), 0);
  sprintf(buf, "\r\n");
  send(client, buf, sizeof(buf), 0);
  sprintf(buf, "<P>Your browser sent a bad request, ");
  send(client, buf, sizeof(buf), 0);
  sprintf(buf, "such as a POST without a Content-Length.</P>\r\n");
  send(client, buf, sizeof(buf), 0);
}

void cannot_execute(int client)
{
  char buf[1024];

  sprintf(buf, "HTTP/1.0 500 Internal Server Error\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "Content-type: text/html\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "\r\n");
  send(client, buf, strlen(buf), 0);
  sprintf(buf, "<P>Error prohibited CGI execution.</P>\r\n");
  send(client, buf, strlen(buf), 0);
}

void execute_cgi(int client, const char *path, const char *method, const char *query_string)
{
  int content_length = -1;

  int numchars = 1;
  char buf[1024] = "A";
  if (strcasecmp(method, "GET") == 0)
    while ((numchars > 0) && strcmp("\n", buf)) // read & discard headers
      numchars = get_line(client, buf, sizeof(buf));
  else // POST
  {
    numchars = get_line(client, buf, sizeof(buf));
    while ((numchars > 0) && strcmp("\n", buf))
    {
      buf[15] = '\0';
      if (strcasecmp(buf, "Content-Length:") == 0)
        content_length = atoi(&(buf[16]));
      numchars = get_line(client, buf, sizeof(buf));
    }

    if (content_length == -1)
    {
      bad_request(client);
      return;
    }
  }

  sprintf(buf, "HTTP/1.0 200 OK\r\n");
  send(client, buf, strlen(buf), 0);

  int cgi_output[2];
  if (pipe(cgi_output) < 0)
  {
    cannot_execute(client);
    return;
  }

  int cgi_input[2];
  if (pipe(cgi_input) < 0)
  {
    cannot_execute(client);
    return;
  }

  pid_t pid;
  if ((pid = fork()) < 0)
  {
    cannot_execute(client);
    return;
  }

  if (pid == 0) // child: CGI script
  {
    dup2(cgi_output[1], 1);
    dup2(cgi_input[0], 0);
    close(cgi_output[0]);
    close(cgi_input[1]);

    char meth_env[255];
    sprintf(meth_env, "REQUEST_METHOD=%s", method);
    putenv(meth_env);

    if (strcasecmp(method, "GET") == 0)
    {
      char query_env[255];
      sprintf(query_env, "QUERY_STRING=%s", query_string);
      putenv(query_env);
    }
    else // POST
    {
      char length_env[255];
      sprintf(length_env, "CONTENT_LENGTH=%d", content_length);
      putenv(length_env);
    }

    execl(path, path, NULL);
    exit(0);
  }
  else // parent
  {
    close(cgi_output[1]);
    close(cgi_input[0]);

    char c;
    if (strcasecmp(method, "POST") == 0)
    {
      for (int i = 0; i < content_length; i++)
      {
        recv(client, &c, 1, 0);
        write(cgi_input[1], &c, 1);
      }
    }
    while (read(cgi_output[0], &c, 1) > 0)
      send(client, &c, 1, 0);

    close(cgi_output[0]);
    close(cgi_input[1]);

    int status;
    waitpid(pid, &status, 0);
  }
}

void accept_request(int client)
{
  char buf[1024];
  int numchars = get_line(client, buf, sizeof(buf));
  size_t i = 0;
  size_t j = 0;
  char method[255];
  while (!ISspace(buf[j]) && (i < sizeof(method) - 1))
    method[i++] = buf[j++];
  method[i] = '\0';

  if (strcasecmp(method, "GET") && strcasecmp(method, "POST"))
  {
    unimplemented(client);
    return;
  }

  int cgi = 0; // Is this a CGIprogram ?
  if (strcasecmp(method, "POST") == 0)
    cgi = 1;

  while (ISspace(buf[j]) && (j < sizeof(buf)))
    j++;

  char url[255];
  i = 0;
  while (!ISspace(buf[j]) && (i < sizeof(url) - 1) && (j < sizeof(buf)))
    url[i++] = buf[j++];
  url[i] = '\0';

  char *query_string = NULL;
  if (strcasecmp(method, "GET") == 0)
  {
    query_string = url;
    while ((*query_string != '?') && (*query_string != '\0'))
      query_string++;
    if (*query_string == '?')
    {
      cgi = 1;
      *query_string = '\0';
      query_string++;
    }
  }

  char path[512];
  sprintf(path, "htdocs%s", url);
  if (path[strlen(path) - 1] == '/')
    strcat(path, "index.html");

  struct stat st;
  if (stat(path, &st) == -1)
  {
    while ((numchars > 0) && strcmp("\n", buf)) // read & discard headers
      numchars = get_line(client, buf, sizeof(buf));
    not_found(client);
  }
  else
  {
    if ((st.st_mode & S_IFMT) == S_IFDIR)
      strcat(path, "/index.html");
    if ((st.st_mode & S_IXUSR) ||
        (st.st_mode & S_IXGRP) ||
        (st.st_mode & S_IXOTH))
      cgi = 1;
    if (!cgi)
      serve_file(client, path);
    else
      execute_cgi(client, path, method, query_string);
  }

  close(client);
}

int main()
{
  u_short port = 0;
  int server_sock = startup(&port);
  printf("httpd running on port %d\n", port);

  while (1)
  {
    struct sockaddr_in client_name;
    int client_name_len = sizeof(client_name);
    int client_sock = accept(server_sock, (struct sockaddr *)&client_name, &client_name_len);
    if (client_sock == -1)
      error_die("accept");

    pthread_t newthread;
    if (pthread_create(&newthread, NULL, accept_request, client_sock) != 0)
      perror("pthread_create");
  }

  close(server_sock);

  return 0;
}
