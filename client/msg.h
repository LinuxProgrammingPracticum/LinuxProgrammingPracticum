/**
 * Login        ：登录
 * Register     ：注册
 * SendToUser   ：向用户发送消息
 * SentToGroup  ：向群组发送消息
 */
enum Type { Login, Register, SendToUser, SendToGroup };

/**
 * command  ： 确定消息类型
 * target   ： 目标（若为向用户发送消息，目标为用户名
 *                  若为向群组发送消息，目标为群组名
 *                  其他可填server，也可不填此选项）
 * me       ： 用户自身
 * buf      ： 内容（若为登录或者注册，则内容填密码）
 */
typedef struct Msg {
    Type command;
    unsigned targetlen;
    char target[20];
    unsigned melen;
    char me[20];
    unsigned buflen;
    char buf[2048];
}msg;
