#ifndef PUBLIC_H
#define PUBLIC_H


//定义事件对应的枚举编号
enum EnMsgType
{
    LOGIN_MSG=1,//登录消息
    LOGIN_MSG_ACK, //登录时发给客户端的消息
    LOGINOUT_MSG,//退出聊天界面的消息
    REG_MSG, //注册消息
    REG_MSG_ACK, //注册时返回给客户端的消息
    ONE_CHAT_MSG, //一对一聊天消息的标识
    ADD_FRIEND_MSG, //添加好友的消息处理

    CREATE_GROUP_MSG,//创建群组的消息处理
    ADD_GROUP_MSG,//群组添加好友的消息处理
    GROUP_CHAT_MSG,//群组聊天的消息处理
};


#endif