#ifndef MSGHANDLE_H
#define MSGHANDLE_H
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "msg.h"
#include "sql.h"
/**
 * 从数据库查找已连接的用户sockfd
 * 参数：用户名
 * 返回值：对应套接字描述符
 */
int findsockfd(char* username) {
    char sql[200];
    sprintf(sql, "select sock from sock where name = \"%s\"", username);
    MYSQL_RES* result = query(sql);
    int rows = mysql_num_rows(result);
    int sockfd = 0;
    if (rows != 0) {
        MYSQL_ROW sqlrow = mysql_fetch_row(result);
        sscanf(sqlrow[0], "%d", &sockfd);
    }
    mysql_free_result(result);
    return sockfd;
}
/**
 * 服务器向客户端发送信息（修改密码成功、查询失败等）
 * 参数：
 *      message ：客户端发送的信息
 *      sockfd  ：与用户连接的套接字描述符
 *      string  ：信息
 */
void infomsg(msg message, int sockfd, char* string) {
    msg info;
    info.command = Info;
    strcpy(info.target, message.me);
    info.targetlen = message.melen;
    strcpy(info.me, "server");
    info.melen = strlen("server");
    strcpy(info.buf, string);
    info.buflen = strlen(string);
    write(sockfd, &info, sizeof(info));
}
/**
 * 登录
 * 功能：向客户端发送结果信息
 * 参数：
 *      message ：客户端发送的信息
 *      sockfd  ：与用户连接的套接字描述符
 * 返回值：
 *      true表示成功，false表示失败
 * （下同）
 */
bool login(msg message, int sockfd) {
    msg info;

    info.command = Info;
    strcpy(info.target, message.me);
    info.targetlen = message.melen;
    strcpy(info.me, "server");
    info.melen = strlen("server");
    char string[45];

    char sql[200];
    sprintf(sql, "select * from user where name = \"%s\" and pwd = \"%s\"",
            message.me, message.buf);
    MYSQL_RES* result = query(sql);
    if (result == NULL) {
        strcpy(string, "error");
        strcpy(info.buf, string);
        info.buflen = strlen(string);
        write(sockfd, &info, sizeof(info));
        mysql_free_result(result);
        return false;
    }

    int rows = mysql_num_rows(result);

    if (rows == 0) {
        strcpy(string, "用户名或密码错误");
        strcpy(info.buf, string);
        info.buflen = strlen(string);
    } else {
        strcpy(string, "success");
        strcpy(info.buf, string);
        info.buflen = strlen(string);
    }
    mysql_free_result(result);

    write(sockfd, &info, sizeof(info));

    return true;
}
/**
 * 注册
 * (此处可复用infomsg代码，可优化)
 */
bool registe(msg message, int sockfd) {
    msg info;

    info.command = Info;
    strcpy(info.target, message.me);
    info.targetlen = message.melen;
    strcpy(info.me, "server");
    info.melen = strlen("server");
    char string[45];

    char sql[200];
    sprintf(sql, "select * from user where name = \"%s\" ", message.me);
    MYSQL_RES* result = query(sql);
    if (result == NULL) {
        strcpy(string, "error");
        strcpy(info.buf, string);
        info.buflen = strlen(string);
        write(sockfd, &info, sizeof(info));
        mysql_free_result(result);
        return false;
    }

    int rows = mysql_num_rows(result);
    mysql_free_result(result);

    if (rows == 0) {
        sprintf(sql, "insert into user(name,pwd) values (\"%s\",\"%s\")",
                message.me, message.buf);
        if (update(sql) == EXIT_SUCCESS) {
            strcpy(string, "创建成功");
            strcpy(info.buf, string);
            info.buflen = strlen(string);
        } else {
            strcpy(string, "创建失败");
            strcpy(info.buf, string);
            info.buflen = strlen(string);
            write(sockfd, &info, sizeof(info));
            return false;
        }
    } else {
        strcpy(string, "该用户已存在");
        strcpy(info.buf, string);
        info.buflen = strlen(string);
    }

    write(sockfd, &info, sizeof(info));

    return true;
}
/**
 * 私聊
 */
bool sendtouser(msg message, int sockfd) {
    write(sockfd, &message, sizeof(message));  //回音
    int targetsock = findsockfd(message.target);
    if (sockfd != 0)
        write(targetsock, &message, sizeof(message));  //若对方在线，发送消息
    //存储到数据库中
    char sql[2048];
    sprintf(sql,
            "insert into userchat(username,targetname,time,content) "
            "values(\"%s\",\"%s\",now(),\"%s\")",
            message.me, message.target, message.buf);
    if (update(sql) == EXIT_SUCCESS)
        return true;
    return false;
}
/**
 * 群聊
 */
bool sendtogroup(msg message, int sockfd) {
    char sql[2048];
    sprintf(sql, "select username from groupmember where groupname = \"%s\"",
            message.target);
    MYSQL_RES* result = query(sql);
    MYSQL_ROW sqlrow;
    int sock;
    while (sqlrow = mysql_fetch_row(result)) {
        sock = findsockfd(sqlrow[0]);
        if (sock != 0) {
            write(sock, &message,
                  sizeof(message));  //向所有在线成员发送（包括自身的回音）
        }
    }

    //存储到数据库中
    memset(sql, '\0', sizeof(sql));
    sprintf(sql,
            "insert into "
            "groupchat(username,groupname,time,content)values(\"%s\",\"%s\","
            "now(),\"%s\")",
            message.me, message.target, message.buf);
    if (update(sql) == EXIT_SUCCESS)
        return true;
    return false;
}
/**
 * 私聊历史信息查询
 */
bool queryhistoryfromuser(msg message, int sockfd) {
    char sql[200];
    msg info;
    info.command = message.command;
    sprintf(sql,
            "select username,targetname,time,content from userchat where  "
            "(username = \"%s\" and targetname = \"%s\")or(username = \"%s\" "
            "and targetname = \"%s\") order by time asc",
            message.me, message.target, message.target, message.me);
    MYSQL_RES* result = query(sql);
    MYSQL_ROW sqlrow;
    int i;
    while (sqlrow = mysql_fetch_row(result)) {
        strcpy(info.me, sqlrow[0]);
        info.melen = strlen(info.me);
        strcpy(info.target, sqlrow[1]);
        info.targetlen = strlen(info.target);
        strcpy(info.buf, sqlrow[2]);
        strcat(info.buf, " : ");
        strcat(info.buf, sqlrow[3]);
        info.buflen = strlen(info.buf);
        write(sockfd, &info, sizeof(msg));
    }
    return true;
}
/**
 * 群聊历史信息查询
 */
bool queryhistoryfromgroup(msg message, int sockfd) {
    char sql[200];
    msg info;
    info.command = message.command;
    sprintf(sql,
            "select username,groupname,time,content from groupchat where "
            "groupname = \"%s\" order by time asc",
            message.target);
    MYSQL_RES* result = query(sql);
    MYSQL_ROW sqlrow;
    int i;
    while (sqlrow = mysql_fetch_row(result)) {
        strcpy(info.me, sqlrow[0]);
        info.melen = strlen(info.me);
        strcpy(info.target, sqlrow[1]);
        info.targetlen = strlen(info.target);
        strcpy(info.buf, sqlrow[2]);
        strcat(info.buf, " : ");
        strcat(info.buf, sqlrow[3]);
        info.buflen = strlen(info.buf);
        write(sockfd, &info, sizeof(msg));
    }
    return true;
}
/**
 * 建群
 */
bool addgroup(msg message, int sockfd);
/**
 * 删群
 */
bool deletegroup(msg message, int sockfd);
/**
 * 加群
 */
bool joingroup(msg message, int sockfd);
/**
 * 退群
 */
bool quitgroup(msg message, int sockfd);
/**
 * 处理信息的总接口
 * 功能：接口
 * 参数：
 *      message ：客户端发送的信息
 *      sockfd  ：与用户连接的套接字描述符
 * 返回值：
 *      true表示成功，false表示失败
 */
bool handle(msg message, int sockfd) {
    bool result;
    switch (message.command) {
        case Login:
            result = login(message, sockfd);
            break;
        case Register:
            result = registe(message, sockfd);
            break;
        case SendToUser:
            result = sendtouser(message, sockfd);
            break;
        case SendToGroup:
            result = sendtogroup(message, sockfd);
            break;
        case queryHistoryFromUser:
            result = queryhistoryfromuser(message, sockfd);
            break;
        case queryHistoryFromGroup:
            result = queryhistoryfromgroup(message, sockfd);
            break;
        case AddGroup:
            result = addgroup(message, sockfd);
            break;
        case DeleteGroup:
            result = deletegroup(message, sockfd);
            break;
        case JoinGroup:
            result = joingroup(message, sockfd);
            break;
        case QuitGroup:
            result = quitgroup(message, sockfd);
            break;
    }
}

#endif