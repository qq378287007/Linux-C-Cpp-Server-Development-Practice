#include <stdio.h>
#include <string.h>
#include <time.h>
#include <mysql.h>

int GetDateTime(char *psDateTime)
{
	time_t nSeconds;
	time(&nSeconds);
	struct tm *pTM = localtime(&nSeconds);

	// 系统日期和时间, yyyy-mm-dd HH:MM:SS
	sprintf(psDateTime,
			"%04d-%02d-%02d %02d:%02d:%02d",
			pTM->tm_year + 1900,
			pTM->tm_mon + 1,
			pTM->tm_mday,
			pTM->tm_hour,
			pTM->tm_min,
			pTM->tm_sec);

	return 0;
}

int insert()
{
	MYSQL mysql;
	mysql_init(&mysql);
	if (!mysql_real_connect(&mysql, "localhost", "root", "123456", "test", 0, NULL, 0))
	{
		printf("Failed to connect to Mysql!\n");
		return 1;
	}
	else
		printf("Connected to Mysql successfully!\n");

	char cur_time[55] = "";
	char buf[512] = "";
	char szName[100] = "Jack2";
	GetDateTime(cur_time);
	sprintf(buf, "INSERT INTO student(name,age,SETTIME) VALUES(\'%s\',%d,\'%s\')", szName, 27, cur_time);
	int r = mysql_query(&mysql, buf);
	if (r)
	{
		printf("Insert data failure!\n");
		return 1;
	}
	else
	{
		printf("Insert data success!\n");
	}

	mysql_close(&mysql);
	return 0;
}

int showTable()
{
	// 初始化连接句柄
	MYSQL mysql;
	mysql_init(&mysql);

	// 连接server
	// MYSQL句柄，serverIP地址，username，password，数据库等
	if (!mysql_real_connect(&mysql, "localhost", "root", "123456", "test", 0, NULL, 0))
		printf("Error connecting to Mysql!\n");
	else
		printf("Connected Mysql successful!\n");

	// 查询，成功0
	char *query = "select * from student";
	int flag = mysql_real_query(&mysql, query, (unsigned int)strlen(query));
	if (flag)
	{
		printf("Query failed!\n");
		return 1;
	}
	else
	{
		printf("[%s] made...\n", query);
	}

	// 读取所有查询结果
	MYSQL_RES *res = mysql_store_result(&mysql);
	do
	{
		// 检索结果集的下一行
		MYSQL_ROW row = mysql_fetch_row(res);
		if (row == 0)
			break;

		// 返回结果集中的字段数目
		for (int t = 0; t < mysql_num_fields(res); t++)
			printf("%s\t", row[t]);
		printf("\n");
	} while (1);

	// 关闭连接
	mysql_close(&mysql);

	return 0;
}

int main()
{
	insert();
	showTable();
	return 0;
}
