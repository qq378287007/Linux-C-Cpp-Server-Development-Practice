#include <stdio.h>
#include <string.h>
#include <mysql.h>

int main()
{
	MYSQL mysql;
	mysql_init(&mysql); // 初始化连接句柄

	// 连接server
	// MYSQL句柄，ip地址，username，password，数据库等
	if (!mysql_real_connect(&mysql, "localhost", "root", "123456", "test", 0, NULL, 0))
		printf("Error connecting to Mysql!\n");
	else
		printf("Connected Mysql successful!\n");

	char *query = "select * from student";
	int flag = mysql_real_query(&mysql, query, (unsigned int)strlen(query)); // 查询，成功0
	if (flag)
	{
		printf("Query failed!\n");
		return 1;
	}
	else
	{
		printf("[%s] made...\n", query);
	}

	MYSQL_RES *res = mysql_store_result(&mysql); // 读取所有查询结果
	int num = mysql_num_fields(res);
	do
	{
		MYSQL_ROW row = mysql_fetch_row(res); // 检索结果集的下一行
		if (row == 0)
			break;

		for (int t = 0; t < num; t++) // 返回结果集中的字段数目
			printf("%s\t", row[t]);
		printf("\n");
	} while (1);

	mysql_close(&mysql); // 关闭连接

	return 0;
}

// sudo apt install libmysqlclient-dev
