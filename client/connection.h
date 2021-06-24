#ifndef CONNECTION_H
#define CONNECTION_H
#include <arpa/inet.h>
#include <msg.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define SERV_PORT 3456
#define IPADDRESS "127.0.0.1"
/**
 * 获得连接
 * 参数： 无
 * 返回值：连接套接字
 */
int getconnection() {
    int sockfd;
    struct sockaddr_in servaddr;
    msg* message;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, IPADDRESS, &servaddr.sin_addr);
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
bool sendmsg(enum Type type,
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
    return sendmsg(Register, sockfd, username, "server", password);
}
/**
 * 登录
 * 参数：连接套接字，用户名，密码
 * 返回值：成功为true，错误为false
 */
bool login(int sockfd, char* username, char* password) {
    return sendmsg(Login, sockfd, username, "server", password);
}
/**
 * 私聊
 * 参数：连接套接字，用户名，目标用户名，内容
 * 返回值：成功为true，错误为false
 */
bool sendtouser(int sockfd, char* username, char* targetname, char* content) {
    return sendmsg(SendToUser, sockfd, username, targetname, content);
}
/**
 * 群聊
 * 参数：连接套接字，用户名，目标群名，内容
 * 返回值：成功为true，错误为false
 */
bool sendtogroup(int sockfd, char* username, char* targetname, char* content) {
    return sendmsg(SendToGroup, sockfd, username, targetname, content);
}
/**
 * 建群
 * 参数：连接套接字，用户名，群名
 * 返回值：成功为true，错误为false
 */
bool addgroup(int sockfd, char* username, char* targetname) {
    return sendmsg(AddGroup, sockfd, username, targetname, "");
}
/**
 * 删群
 * 参数：连接套接字，用户名，群名
 * 返回值：成功为true，错误为false
 */
bool deletegroup(int sockfd, char* username, char* targetname) {
    return sendmsg(DeleteGroup, sockfd, username, targetname, "");
}
/**
 * 加群
 * 参数：连接套接字，用户名，群名
 * 返回值：成功为true，错误为false
 */
bool joingroup(int sockfd, char* username, char* targetname) {
    return sendmsg(JoinGroup, sockfd, username, targetname, "");
}
/**
 * 退群
 * 参数：连接套接字，用户名，群名
 * 返回值：成功为true，错误为false
 */
bool quitgroup(int sockfd, char* username, char* targetname) {
    return sendmsg(QuitGroup, sockfd, username, targetname, "");
}
/**
 * 查询用户聊天历史信息
 * 参数：连接套接字，用户名，目标用户名
 * 返回值：成功为true，错误为false
 */
bool queryhistoryfromuser(int sockfd, char* username, char* targetname) {
    return sendmsg(QueryHistoryFromUser, sockfd, username, targetname, "");
}
/**
 * 查询群组聊天历史信息
 * 参数：连接套接字，用户名，目标用户名
 * 返回值：成功为true，错误为false
 */
bool queryhistoryfromgroup(int sockfd, char* username, char* targetname) {
    return sendmsg(QueryHistoryFromGroup, sockfd, username, targetname, "");
}
#endif