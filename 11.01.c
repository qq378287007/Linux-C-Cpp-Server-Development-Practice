#include <stdio.h>
#include <string.h>
#include <mysql.h>

int main()
{
	MYSQL mysql;
	mysql_init(&mysql);// 初始化连接句柄

	// 连接server
	// MYSQL句柄，serverIP地址，username，password，数据库等
	if (!mysql_real_connect(&mysql, "localhost", "root", "mali", "test", 0, NULL, 0))
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
