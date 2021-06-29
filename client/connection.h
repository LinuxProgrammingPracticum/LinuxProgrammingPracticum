#ifndef CONNECTION_H
#define CONNECTION_H
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "msg.h"
#define SERV_PORT 3456
/**
 * 获得连接
 * 参数： IP地址
 * 返回值：连接套接字
 */
int getconnection(char* ipaddress) {
    int sockfd;
    struct sockaddr_in servaddr;
    msg* message;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, ipaddress, &servaddr.sin_addr);
    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
        return EXIT_FAILURE;
    return sockfd;
}
/**
 * 关闭连接
 * 参数： 连接套接字
 * 返回值：无
 */
void closeconnection(int sockfd) {
    close(sockfd);
}
/**
 * 发送消息
 * 参数：类型、连接套接字、自身名称、目标名称、内容
 * 返回值：成功为true，错误为false
 */
bool sendMsg(enum Type type,
             int sockfd,
             char* me,
             char* target,
             char* content) {
    msg message;
    strcpy(message.me, me);
    strcpy(message.target, target);
    strcpy(message.buf, content);
    message.melen = strlen(me);
    message.targetlen = strlen(target);
    message.buflen = strlen(content);
    message.command = type;
    int result = write(sockfd, &message, sizeof(message));
    if (result == -1)
        return false;
    return true;
}
/**
 * 接收来自服务器的消息
 * 参数：连接套接字，消息指针（用于写回）
 */
void receivemsg(int sockfd, msg* message) {
    read(sockfd, message, sizeof(msg));
}
/**
 * 注册
 * 参数：连接套接字，用户名，密码
 * 返回值：成功为true，错误为false
 */
bool registe(int sockfd, char* username, char* password) {
    return sendMsg(Register, sockfd, username, "server", password);
}
/**
 * 登录
 * 参数：连接套接字，用户名，密码
 * 返回值：成功为true，错误为false
 */
bool login(int sockfd, char* username, char* password) {
    return sendMsg(Login, sockfd, username, "server", password);
}
/**
 * 私聊
 * 参数：连接套接字，用户名，目标用户名，内容
 * 返回值：成功为true，错误为false
 */
bool sendtouser(int sockfd, char* username, char* targetname, char* content) {
    return sendMsg(SendToUser, sockfd, username, targetname, content);
}
/**
 * 群聊
 * 参数：连接套接字，用户名，目标群名，内容
 * 返回值：成功为true，错误为false
 */
bool sendtogroup(int sockfd, char* username, char* targetname, char* content) {
    return sendMsg(SendToGroup, sockfd, username, targetname, content);
}
/**
 * 建群
 * 参数：连接套接字，用户名，群名
 * 返回值：成功为true，错误为false
 */
bool addgroup(int sockfd, char* username, char* targetname) {
    return sendMsg(AddGroup, sockfd, username, targetname, "");
}
/**
 * 删群
 * 参数：连接套接字，用户名，群名
 * 返回值：成功为true，错误为false
 */
bool deletegroup(int sockfd, char* username, char* targetname) {
    return sendMsg(DeleteGroup, sockfd, username, targetname, "");
}
/**
 * 加群
 * 参数：连接套接字，用户名，群名
 * 返回值：成功为true，错误为false
 */
bool joingroup(int sockfd, char* username, char* targetname) {
    return sendMsg(JoinGroup, sockfd, username, targetname, "");
}
/**
 * 退群
 * 参数：连接套接字，用户名，群名
 * 返回值：成功为true，错误为false
 */
bool quitgroup(int sockfd, char* username, char* targetname) {
    return sendMsg(QuitGroup, sockfd, username, targetname, "");
}
/**
 * 查询用户聊天历史信息
 * 参数：连接套接字，用户名，目标用户名
 * 返回值：成功为true，错误为false
 */
bool queryhistoryfromuser(int sockfd, char* username, char* targetname) {
    return sendMsg(QueryHistoryFromUser, sockfd, username, targetname, "");
}
/**
 * 查询群组聊天历史信息
 * 参数：连接套接字，用户名，目标用户名
 * 返回值：成功为true，错误为false
 */
bool queryhistoryfromgroup(int sockfd, char* username, char* targetname) {
    return sendMsg(QueryHistoryFromGroup, sockfd, username, targetname, "");
}
/**
 * 添加好友
 * 参数：连接套接字，用户名，好友用户名
 * 返回值：成功为true，错误为false
 */
bool addfriend(int sockfd, char* username, char* targetname) {
    return sendMsg(AddFriend, sockfd, username, targetname, "");
}
/**
 * 删除好友
 * 参数：连接套接字，用户名，好友用户名
 * 返回值：成功为true，错误为false
 */
bool deletefriend(int sockfd, char* username, char* targetname) {
    return sendMsg(DeleteFriend, sockfd, username, targetname, "");
}
/**
 * 查询好友列表
 * 参数：连接套接字，用户名
 * 返回值：成功为true，错误为false
 */
bool queryfriendlist(int sockfd, char* username) {
    return sendMsg(QueryFriendList, sockfd, username, "", "");
}
/**
 * 查询群组列表
 * 参数：连接套接字，用户名
 * 返回值：成功为true，错误为false
 */
bool querygrouplist(int sockfd, char* username) {
    return sendMsg(QueryGroupList, sockfd, username, "", "");
}
#endif