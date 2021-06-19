#ifndef SQL_H
#define SQL_H
#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>

#define IP_ADDRESS "dpnbb.top"
#define USER "user"
#define PASSWORD "LinuxTest666*"
#define DBNAME "LinuxProgrammingPracticum"
/**
 *数据库增删改操作
 *参数：SQL语句
 *返回值：成功为EXIT_SUCCESS，错误为WXIT_FAILURE
 */
int update(char* sql) {
    MYSQL* conn_ptr;
    conn_ptr = mysql_init(NULL);

    if (!conn_ptr) {
        printf("SELECT error\n");
        printf("mysql_init failed\n");
        return EXIT_FAILURE;
    }

    conn_ptr = mysql_real_connect(conn_ptr, IP_ADDRESS, USER, PASSWORD, DBNAME,
                                  0, NULL, 0);

    if (conn_ptr) {
        // printf("Connection success\n");
    } else {
        printf("Connection failed\n");
        mysql_close(conn_ptr);
        return EXIT_FAILURE;
    }

    int res, i, j;
    res = mysql_query(conn_ptr, sql);
    if (res) {
        printf("update error\n");
        mysql_close(conn_ptr);
        return EXIT_FAILURE;
    }
    mysql_close(conn_ptr);
    return EXIT_SUCCESS;
}
/**
 *数据库查询操作
 *参数：SQL语句
 *返回值：数据库查询结果，错误为NULL
 *注意：在调用此函数后，应当对返回值进行一次mysql_free_result操作
 */
MYSQL_RES* query(char* sql) {
    MYSQL* conn_ptr;
    MYSQL_RES* res_ptr;
    MYSQL_ROW sqlrow;
    MYSQL_FIELD* fd;

    conn_ptr = mysql_init(NULL);

    if (!conn_ptr) {
        printf("mysql_init failed\n");
        return NULL;
    }

    conn_ptr = mysql_real_connect(conn_ptr, IP_ADDRESS, USER, PASSWORD, DBNAME,
                                  0, NULL, 0);

    if (conn_ptr) {
        printf("Connection success\n");
    } else {
        printf("Connection failed\n");
        mysql_close(conn_ptr);
        return NULL;
    }

    int res, i, j;
    res = mysql_query(conn_ptr, sql);
    if (res) {
        printf("SELECT error\n");
    } else {
        res_ptr = mysql_store_result(conn_ptr);
        if (mysql_errno(conn_ptr)) {
            fprintf(stderr, "Retrive error:s\n", mysql_error(conn_ptr));
        }
        // mysql_free_result(res_ptr);
    }

    mysql_close(conn_ptr);
    return res_ptr;
}
#endif