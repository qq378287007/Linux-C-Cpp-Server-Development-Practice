#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <time.h>
#include <winsock2.h>
#include <windows.h>

#include <iostream>
#include <string>
#include <vector>
using namespace std;

//#define IP "127.0.0.1"
#define IP "192.168.234.128"
#define PORT 8000
#define BUF_SIZE 1024
#define NAME_SIZE 20

void ErrorHanding(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void Send(SOCKET hSock, const string &message)
{
    send(hSock, message.c_str(), message.size(), 0);
}

vector<string> reply;

unsigned WINAPI recv_msg(void *arg)
{
	SOCKET sock = *((SOCKET *)arg);
	while (1)
	{
		char name_msg[NAME_SIZE + BUF_SIZE] = {0};
		int str_len = recv(sock, name_msg, NAME_SIZE + BUF_SIZE - 1, 0);
		if (str_len == -1)
			return -1;
		else if (str_len == 0)
			break;
		else
		{
			name_msg[str_len] = 0;
			//fputs(name_msg, stdout);
            string message = name_msg;
            reply.emplace_back(message);
		}
	}
	closesocket(sock);
	return 0;
}


int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHanding("WSAStartup() error!");

    SOCKET hSock = socket(PF_INET, SOCK_STREAM, 0);
    if (hSock == INVALID_SOCKET)
        ErrorHanding("socket() error!");
    int szAddr = sizeof(SOCKADDR_IN);

    SOCKADDR_IN servAddr;
    memset(&servAddr, 0, szAddr);
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(IP);
    servAddr.sin_port = htons(PORT);

    if (connect(hSock, (SOCKADDR *)&servAddr, szAddr) == SOCKET_ERROR)
        ErrorHanding("connect() error!");
    else
        puts("Connected.......");

    HANDLE rcv_thread = (HANDLE)_beginthreadex(NULL, 0, recv_msg, (void *)&hSock, 0, NULL);

    while (true)
    {
        string command;
        cout << "Please input command: ";
        getline(cin, command);

        if (command == "register")
        {
            string name;
            cout << "Please input name: ";
            getline(cin, name);
            name = "r," + name;
            Send(hSock, name);
        }
        else if (command == "login")
        {
            string name;
            cout << "Please input name: ";
            getline(cin, name);
            name = "l," + name;
            Send(hSock, name);
        }
        else if (command == "chat")
        {
            string content;
            cout << "Please input content: ";
            getline(cin, content);
            content = "c," + content;
            Send(hSock, content);
        }
        else if (command == "record")
        {
            cout << "Record\n";
            for (auto record : reply)
                cout << "\t" << record << endl;
        }
        else if (command == "exit")
        {
            break;
        }
    }

	WaitForSingleObject(rcv_thread, INFINITE);

    closesocket(hSock);

    WSACleanup();

    return 0;
}

// g++ myChatCln.cpp -o myChatCln -lws2_32 & myChatCln.exe
