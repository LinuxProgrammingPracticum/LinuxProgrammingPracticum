#ifndef MSG_H
#define MSG_H
/**
 * Login                ：登录
 * Register             ：注册
 * SendToUser           ：向用户发送消息
 * SentToGroup          ：向群组发送消息
 * queryHistoryFromUser ：查找与用户的历史消息
 * queryHistoryFromGroup：查找群聊历史消息
 * AddGroup             ：建群
 * DeleteGroup          ：删群
 * JoinGroup            ：加群
 * QuitGroup            ：退群
 * Info                 ：服务器发送消息或用户发送关闭连接
 */
enum Type {
    Login,
    Register,
    SendToUser,
    SendToGroup,
    QueryHistoryFromUser,
    QueryHistoryFromGroup,
    AddGroup,
    DeleteGroup,
    JoinGroup,
    QuitGroup,
    Info,
};

/**
 * command  ： 确定消息类型
 * target   ： 目标（若为向用户发送消息或查找用户历史消息，目标为用户名
 *                  若为向群组发送消息或查找群组历史消息，抑或是加群和退群，抑或是建群和删群，目标为群组名
 *                  其他可填server，也可不填此选项）
 * me       ： 用户自身
 * buf      ： 内容（若为登录或者注册，则内容填密码）
 */
typedef struct Msg {
    enum Type command;
    unsigned targetlen;
    char target[20];
    unsigned melen;
    char me[20];
    unsigned buflen;
    char buf[2048];
} msg;
#endif