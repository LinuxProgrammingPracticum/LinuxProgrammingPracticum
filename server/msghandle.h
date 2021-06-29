#ifndef MSGHANDLE_H
#define MSGHANDLE_H
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "msg.h"
#include "sql.h"
/**
 * 向数据库插入sockfd
 * 参数：文件描述符，用户名
 * 返回值：成功为true，错误为false
 */
bool insertsock(int sockfd, char* name) {
    char sql[200];
    sprintf(sql, "insert into sock(sock,name)values(\"%d\",\"%s\")", sockfd,
            name);
    return (update(sql));
}
/**
 * 向数据库删除sockfd
 * 参数：文件描述符
 * 返回值：成功为true，错误为false
 */
bool deletesock(int sockfd) {
    char sql[200];
    sprintf(sql, "delete from sock where sock = \"%d\"", sockfd);
    return (update(sql));
}
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
bool infomsg(msg message, enum Type type, int sockfd, char* string) {
    msg info;
    info.command = type;
    strcpy(info.target, message.me);
    info.targetlen = message.melen;
    strcpy(info.me, "server");
    info.melen = strlen("server");
    strcpy(info.buf, string);
    info.buflen = strlen(string);
    int result = write(sockfd, &info, sizeof(info));
    if (result == -1)
        return false;
    return true;
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

    info.command = message.command;
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

    int n;
    n = write(sockfd, &info, sizeof(info));
    if (n == -1)
        return false;
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
    //查找是否已有该用户
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
    //添加新用户
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

    int n;
    n = write(sockfd, &info, sizeof(info));
    if (n == -1)
        return false;
    return true;
}
/**
 * 私聊
 */
bool sendtouser(msg message, int sockfd) {
    write(sockfd, &message, sizeof(message));  //回音
    int targetsock = findsockfd(message.target);
    int result = 0;
    char sql[2048];
    if (targetsock != 0)
        result = write(targetsock, &message,
                       sizeof(message));  //若对方在线，发送消息
    else {
        sprintf(sql,
                "insert into unheard(name,targetname,type) "
                "values(\"%s\",\"%s\",\"%s\")",
                message.me, message.target, "user");
        if (update(sql) == EXIT_SUCCESS)
            memset(sql, '\0', sizeof(sql));
        else {
            return false;
        }
    }
    //存储到数据库中
    sprintf(sql,
            "insert into userchat(username,targetname,time,content) "
            "values(\"%s\",\"%s\",now(),\"%s\")",
            message.me, message.target, message.buf);
    if (update(sql) == EXIT_SUCCESS && result != -1)
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
    MYSQL_RES* result;
    result = query(sql);
    memset(sql, '\0', sizeof(sql));
    MYSQL_ROW sqlrow;
    int sock;
    int n;
    n = 0;
    while (sqlrow = mysql_fetch_row(result)) {
        sock = findsockfd(sqlrow[0]);
        if (sock != 0) {
            //向所有在线成员发送（包括自身的回音）
            n = write(sock, &message, sizeof(message));
        } else {
            //不在线就存进未接收消息表
            sprintf(sql,
                    "insert into unheard(name,targetname,type) "
                    "values(\"%s\",\"%s\",\"%s\")",
                    message.me, sqlrow[0], "group");
            if (update(sql) == EXIT_SUCCESS)
                memset(sql, '\0', sizeof(sql));
        }
    }
    mysql_free_result(result);

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
    int n;
    n = 0;
    while (sqlrow = mysql_fetch_row(result)) {
        strcpy(info.me, sqlrow[0]);
        info.melen = strlen(info.me);
        strcpy(info.target, sqlrow[1]);
        info.targetlen = strlen(info.target);
        strcpy(info.buf, sqlrow[2]);
        strcat(info.buf, " : ");
        strcat(info.buf, sqlrow[3]);
        info.buflen = strlen(info.buf);
        n = write(sockfd, &info, sizeof(msg));
    }
    infomsg(message, Info, sockfd, "查询完毕");
    mysql_free_result(result);
    if (n == -1)
        return false;
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
    MYSQL_RES* result;
    result = query(sql);
    MYSQL_ROW sqlrow;
    int i;
    int n;
    n = 0;
    while (sqlrow = mysql_fetch_row(result)) {
        strcpy(info.me, sqlrow[0]);
        info.melen = strlen(info.me);
        strcpy(info.target, sqlrow[1]);
        info.targetlen = strlen(info.target);
        strcpy(info.buf, sqlrow[2]);
        strcat(info.buf, " : ");
        strcat(info.buf, sqlrow[3]);
        info.buflen = strlen(info.buf);
        n = write(sockfd, &info, sizeof(msg));
    }
    mysql_free_result(result);
    infomsg(message, Info, sockfd, "查询完毕");
    if (n == -1)
        return false;
    return true;
}
/**
 * 建群
 * 当发送的message类型为Info表示失败，为AddGroup表示成功
 */
bool addgroup(msg message, int sockfd) {
    char sql[200];
    bool n;
    //查询是否已有该群
    sprintf(sql, "select * from groupt where name = \"%s\"");
    MYSQL_RES* result;
    result = query(sql);
    int rows = mysql_num_rows(result);
    mysql_free_result(result);
    if (rows != 0) {
        n = infomsg(message, Info, sockfd, "该群已存在");
        return n;
    }
    //加群
    memset(sql, '\0', sizeof(sql));
    sprintf(sql, "insert into groupt (name) values(\"%s\")", message.target);
    if (update(sql) == EXIT_FAILURE)
        dprintf(STDERR_FILENO, "%s error\n", sql);
    //加群主
    memset(sql, '\0', sizeof(sql));
    sprintf(sql,
            "insert into groupmember(username,groupname,isadmin) values "
            "(\"%s\",\"%s\",\"%s\");",
            message.me, message.target, "yes");
    if (update(sql) == EXIT_FAILURE)
        dprintf(STDERR_FILENO, "%s error\n", sql);
    n = infomsg(message, Info, sockfd, "成功");
    return n;
}
/**
 * 删群
 */
bool deletegroup(msg message, int sockfd) {
    char sql[200];
    bool n;
    //查询是否为群主
    sprintf(sql,
            "select * from groupmember where username = \"%s\" and groupname = "
            "\"%s\" and isadmin = \"%s\"",
            message.me, message.target, "yes");
    MYSQL_RES* result = query(sql);
    int rows = mysql_num_rows(result);
    mysql_free_result(result);
    if (rows != 0) {  //是群主
        memset(sql, '\0', sizeof(sql));
        sprintf(sql, "delete from groupmember where groupname = \"%s\"",
                message.target);
        if (update(sql) == EXIT_FAILURE)
            dprintf(STDERR_FILENO, "%s error\n", sql);
        memset(sql, '\0', sizeof(sql));
        sprintf(sql, "delete from groupt where name = \"%s\"", message.target);
        if (update(sql) == EXIT_FAILURE)
            dprintf(STDERR_FILENO, "%s error\n", sql);
        n = infomsg(message, Info, sockfd, "删除成功");
    } else {
        n = infomsg(message, Info, sockfd, "您非群主");
    }
    return n;
}
/**
 * 加群
 */
bool joingroup(msg message, int sockfd) {
    char sql[200];
    bool n;
    //查询是否已在组内
    sprintf(sql,
            "select * from groupmember where username = \"%s\" and groupname = "
            "\"%s\"",
            message.me, message.target);
    MYSQL_RES* result;
    result = query(sql);
    int rows = mysql_num_rows(result);
    mysql_free_result(result);
    if (rows == 0) {
        memset(sql, '\0', sizeof(sql));
        sprintf(sql,
                "insert into groupmember(username,groupname,isadmin) values "
                "(\"%s\",\"%s\",\"%s\")",
                message.me, message.target, "np");
        if (update(sql) == EXIT_FAILURE)
            dprintf(STDERR_FILENO, "%s error\n", sql);
        n = infomsg(message, Info, sockfd, "加群成功");
    } else {
        n = infomsg(message, Info, sockfd, "已在群中");
    }
    return n;
}
/**
 * 退群
 */
bool quitgroup(msg message, int sockfd) {
    char sql[200];
    bool n;
    //查询是否已在组内
    sprintf(sql,
            "select * from groupmember where username = \"%s\" and groupname = "
            "\"%s\"",
            message.me, message.target);
    MYSQL_RES* result;
    result = query(sql);
    int rows = mysql_num_rows(result);
    mysql_free_result(result);
    if (rows != 0) {
        memset(sql, '\0', sizeof(sql));
        sprintf(sql,
                "delete from groupmember where username = \"%s\" and groupname "
                "= \"%s\"",
                message.me, message.target);
        if (update(sql) == EXIT_FAILURE)
            dprintf(STDERR_FILENO, "%s error\n", sql);
        n = infomsg(message, Info, sockfd, "退群成功");
    } else {
        n = infomsg(message, Info, sockfd, "不在群中");
    }
    return n;
}
/**
 * 加好友
 */
bool addfriend(msg message, int sockfd) {
    char sql[200];
    sprintf(
        sql,
        "select * from friend where username = \"%s\" and targetname = \"%s\"",
        message.me, message.target);
    MYSQL_RES* result = query(sql);
    if (result == NULL) {
        infomsg(message, Info, sockfd, "error");
        return false;
    }
    int rows = mysql_num_rows(result);
    mysql_free_result(result);
    if (rows == 0) {
        memset(sql, 0, sizeof(sql));
        sprintf(sql,
                "insert into friend(username,targetname)values(\"%s\",\"%s\")",
                message.me, message.target);
        if (update(sql) == EXIT_FAILURE) {
            infomsg(message, Info, sockfd, "数据库更新出错");
            return false;
        }
        memset(sql, 0, sizeof(sql));
        sprintf(sql,
                "insert into friend(username,targetname) values(\"%s\",\"%s\")",
                message.target, message.me);
        if (update(sql) == EXIT_FAILURE) {
            infomsg(message, Info, sockfd, "数据库更新出错");
            return false;
        }
        infomsg(message, message.command, sockfd, "添加好友成功");
        return true;
    } else {
        infomsg(message, Info, sockfd, "已为好友");
        return true;
    }
}
/**
 * 删除好友
 */
bool deletefriend(msg message, int sockfd) {
    char sql[200];
    sprintf(
        sql,
        "select * from friend where username = \"%s\" and targetname = \"%s\"",
        message.me, message.target);
    MYSQL_RES* result = query(sql);
    if (result == NULL) {
        infomsg(message, Info, sockfd, "error");
        return false;
    }
    int rows = mysql_num_rows(result);
    mysql_free_result(result);
    if (rows == 0) {
        infomsg(message, Info, sockfd, "不是好友");
        return true;
    } else {
        memset(sql, 0, sizeof(sql));
        sprintf(sql,
                "delete from friend where username = \"%s\" and targetname = "
                "\"%s\"",
                message.me, message.target);
        if (update(sql) == EXIT_FAILURE) {
            infomsg(message, Info, sockfd, "数据库更新出错");
            return false;
        }
        memset(sql, 0, sizeof(sql));
        sprintf(sql,
                "delete from friend where username = \"%s\" and targetname = "
                "\"%s\"",
                message.target, message.me);
        if (update(sql) == EXIT_FAILURE) {
            infomsg(message, Info, sockfd, "数据库更新出错");
            return false;
        }
        infomsg(message, message.command, sockfd, "删除好友成功");
        return true;
    }
}
/**
 * 查询好友列表
 */
bool queryfriendlist(msg message, int sockfd) {
    char sql[200];
    sprintf(sql, "select targetname from friend where username = \"%s\"",
            message.me);
    MYSQL_RES* result = query(sql);
    if (result == NULL) {
        infomsg(message, Info, sockfd, "数据库查询出错");
        return false;
    }
    MYSQL_ROW sqlrow;
    while (sqlrow = mysql_fetch_row(result)) {
        infomsg(message, message.command, sockfd, sqlrow[0]);
    }
    mysql_free_result(result);
    infomsg(message, Info, sockfd, "查询完毕");
    return true;
}
/**
 * 查询群组列表
 */
bool querygrouplist(msg message, int sockfd) {
    char sql[200];
    sprintf(sql, "select groupname from groupmember where username = \"%s\"",
            message.me);
    MYSQL_RES* result = query(sql);
    if (result == NULL) {
        infomsg(message, Info, sockfd, "数据库查询出错");
        return false;
    }
    MYSQL_ROW sqlrow;
    while (sqlrow = mysql_fetch_row(result)) {
        infomsg(message, message.command, sockfd, sqlrow[0]);
    }
    mysql_free_result(result);
    infomsg(message, Info, sockfd, "查询完毕");
    return true;
}
/**
 * 查询未接收消息列表
 */
bool queryunheard(msg message, int sockfd) {
    char sql[200];
    char buf[1024];
    sprintf(buf, "来自用户的未读消息：");
    sprintf(
        sql,
        "select name from unheard where targetname = \"%s\" and type = \"%s\"",
        message.me, "user");
    MYSQL_RES* result = query(sql);
    memset(sql, '\0', sizeof(sql));
    if (result == NULL) {
        infomsg(message, Info, sockfd, "数据库查询出错");
        return false;
    }
    MYSQL_ROW sqlrow;
    int rows = mysql_num_rows(result);
    if (rows == 0)
        strcat(buf, "无");
    while (sqlrow = mysql_fetch_row(result)) {
        strcat(buf, " ");
        strcat(buf, sqlrow[0]);
    }
    mysql_free_result(result);
    strcat(buf, "\n来自群的未读消息：");
    sprintf(
        sql,
        "select name from unheard where targetname = \"%s\" and type = \"%s\"",
        message.me, "group");
    result = query(sql);
    memset(sql, '\0', sizeof(sql));
    if (result == NULL) {
        infomsg(message, Info, sockfd, "数据库查询出错");
        return false;
    }
    rows = mysql_num_rows(result);
    if (rows == 0)
        strcat(buf, "无");
    while (sqlrow = mysql_fetch_row(result)) {
        strcat(buf, " ");
        strcat(buf, sqlrow[0]);
    }
    mysql_free_result(result);
    sprintf(sql, "delete from unheard where targetname = \"%s\"", message.me);
    if (update(sql) == EXIT_SUCCESS)
        memset(sql, '\0', sizeof(sql));
    if (infomsg(message, message.command, sockfd, buf)) {
        return true;
    }
    return false;
}
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
            if (findsockfd(message.me) == 0)
                insertsock(sockfd, message.me);
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
        case QueryHistoryFromUser:
            result = queryhistoryfromuser(message, sockfd);
            break;
        case QueryHistoryFromGroup:
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
        case AddFriend:
            result = addfriend(message, sockfd);
            break;
        case DeleteFriend:
            result = deletefriend(message, sockfd);
            break;
        case QueryFriendList:
            result = queryfriendlist(message, sockfd);
            break;
        case QueryGroupList:
            result = querygrouplist(message, sockfd);
            break;
        case QueryUnHeard:
            result = queryunheard(message, sockfd);
            break;
    }
    return result;
}

#endif