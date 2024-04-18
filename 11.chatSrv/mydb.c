#include <stdio.h>
#include <string.h>
#include <time.h>
#include <mysql.h>

int insert(char *name)
{
	MYSQL mysql;
	mysql_init(&mysql);

	if (!mysql_real_connect(&mysql, "localhost", "root", "123456", "chatdb", 0, NULL, 0))
	{
		printf("Failed to connect to Mysql!\n");
		mysql_close(&mysql);
		return 1;
	}
	printf("Connected to Mysql successfully!\n");

	char buf[512] = {0};
	sprintf(buf, "INSERT INTO qqnum(name) VALUES(\'%s\')", name);
	int r = mysql_query(&mysql, buf);
	if (r)
	{
		printf("Insert data failure!\n");
		mysql_close(&mysql);
		return 1;
	}
	printf("Insert data success!\n");

	mysql_close(&mysql);
	return 0;
}

int IsExist(char *name)
{
	int res = 0;

	MYSQL mysql;
	mysql_init(&mysql);
	if (!mysql_real_connect(&mysql, "localhost", "root", "123456", "chatdb", 0, NULL, 0))
	{
		printf("Failed to connect to Mysql!\n");
		res = -1;
		goto end;
	}
	printf("Connected to Mysql successfully!\n");

	char buf[512] = "";
	sprintf(buf, "select name from qqnum where name ='%s'", name);
	if (mysql_query(&mysql, buf))
	{
		res = -1;
		goto end;
	}

	MYSQL_RES *result = mysql_store_result(&mysql);
	if (result == NULL)
	{
		res = -1;
		goto end;
	}

	MYSQL_ROW row = mysql_fetch_row(result);
	if (row > 0)
	{
		printf("%s\n", row[0]);
		res = 1;
		goto end;
	}

end:
	mysql_close(&mysql);
	return res;
}

int showTable()
{
	MYSQL mysql;
	mysql_init(&mysql);
	
	if (!mysql_real_connect(&mysql, "localhost", "root", "123456", "chatdb", 0, NULL, 0))
		printf("Error connecting to Mysql!\n");
	printf("Connected Mysql successful!\n");

	char *query = "select * from qqnum";
	int flag = mysql_real_query(&mysql, query, (unsigned int)strlen(query));
	if (flag)
	{
		printf("Query failed!\n");
		return 0;
	}
	printf("[%s] made...\n", query);

	MYSQL_RES *res = mysql_store_result(&mysql);
	int num = mysql_num_fields(res);
	do
	{
		MYSQL_ROW row = mysql_fetch_row(res);
		if (row == 0)
			break;
		for (int t = 0; t < num; t++)
			printf("%s\t", row[t]);
		printf("\n");
	} while (1);

	mysql_close(&mysql);
	return 0;
}
