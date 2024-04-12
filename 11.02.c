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

int insert(char *name, int age)
{
	int status = 0;

	MYSQL mysql;
	mysql_init(&mysql);
	if (!mysql_real_connect(&mysql, "localhost", "root", "123456", "test", 0, NULL, 0))
	{
		printf("Failed to connect to Mysql!\n");
		status = 1;
		goto end;
	}
	printf("Connected to Mysql successfully!\n");

	char buf[512] = "";
	char cur_time[55] = "";
	GetDateTime(cur_time);
	sprintf(buf, "INSERT INTO student(name, age, SETTIME) VALUES(\'%s\', %d, \'%s\')", name, age, cur_time);
	int r = mysql_query(&mysql, buf);
	if (r)
	{
		printf("Insert data failure!\n");
		status = 1;
		goto end;
	}
	printf("Insert data success!\n");

end:
	mysql_close(&mysql);
	return status;
}

int showTable()
{
	int status = 0;

	MYSQL mysql;
	mysql_init(&mysql);

	if (!mysql_real_connect(&mysql, "localhost", "root", "123456", "test", 0, NULL, 0))
	{
		printf("Error connecting to Mysql!\n");
		status = 1;
		goto end;
	}
	printf("Connected Mysql successful!\n");

	char *query = "select * from student";
	int flag = mysql_real_query(&mysql, query, (unsigned int)strlen(query));
	if (flag)
	{
		printf("Query failed!\n");
		status = 1;
		goto end;
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

end:
	mysql_close(&mysql);
	return status;
}

int main()
{
	char name[] = "Jack2";
	int age = 27;
	insert(name, age);
	showTable();
	return 0;
}
