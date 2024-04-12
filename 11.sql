DROP DATABASE IF EXISTS test;
create database test default character set utf8 collate utf8_bin;

flush privileges;

use test;
SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

DROP TABLE IF EXISTS `student`;
CREATE TABLE `student` (
	`id` tinyint NOT NULL AUTO_INCREMENT COMMENT '学生id',
	`name` varchar(32) NOT NULL COMMENT '学生名称',
	`age` smallint DEFAULT NULL COMMENT '年龄',
	`SETTIME` datetime NOT NULL COMMENT '入学时间',
	PRIMARY KEY(`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

BEGIN;
INSERT INTO `student` VALUES (1,'张三',23,'2020-09-30 14:18:32');
INSERT INTO `student` VALUES (2,'李四',22,'2020-09-30 15:18:32');
COMMIT;

SET FOREIGN_KEY_CHECKS = 1;

-- source 11.sql;

-- mysql -h hostname -u username -p database_name < your_sql_file.sql