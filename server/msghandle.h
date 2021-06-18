#ifndef MSGHANDLE_H
#define MSGHANDLE_H
#include <stdbool.h>
#include <string.h>
#include "msg.h"
/**
 * 向客户端返回错误信息
 * 参数：
 *      message ：客户端发送的信息
 *      sockfd  ：与用户连接的套接字描述符
 *      string  ：错误信息
 */
void errormsg(msg message, int sockfd, char* string);
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
bool login(msg message, int sockfd);
/**
 * 注册
 */
bool registe(msg message, int sockfd);
/**
 * 私聊
 */
bool sendtouser(msg message, int sockfd);
/**
 * 群聊
 */
bool sendtogroup(msg message, int sockfd);
/**
 * 私聊历史信息查询
 */
bool querehistoryfromuser(msg message, int sockfd);
/**
 * 群聊历时信息查询
 */
bool querehistoryfromgroup(msg message, int sockfd);
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
        case QuereHistoryFromUser:
            result = querehistoryfromuser(message, sockfd);
            break;
        case QuereHistoryFromGroup:
            result = querehistoryfromgroup(message, sockfd);
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