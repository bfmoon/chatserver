#ifndef CHATSERVICE_H
#define CHATSERVICE_H
#include<muduo/net/TcpConnection.h>

#include<unordered_map>
#include<functional>
#include<mutex>
using namespace std;
using namespace muduo;
using namespace muduo::net;

#include"json.hpp"
using json=nlohmann::json;

#include"redis.hpp"
#include"usermodel.hpp"
#include"offlinemessageModel.hpp"
#include"friendmodel.hpp"
#include"groupmodel.hpp"

//然后这里是业务逻辑的类，以及功能的实现

//定义id对应的业务逻辑的实现
using MsgHandler=function<void(const TcpConnectionPtr& conn,json& js,Timestamp)>;

//定义聊天服务器的业务模块
//利用单例模式
class ChatService
{
public:
    //单例模式获取对象指针的方法
    static ChatService* instance();
    //登录事件
    void login(const TcpConnectionPtr& conn,json& js,Timestamp time);
    //注册事件
    void reg(const TcpConnectionPtr& conn,json& js,Timestamp time);
    //一对一聊天事件
    void oneChat(const TcpConnectionPtr& conn,json& js,Timestamp time);
    //添加好友事件
    void addFriend(const TcpConnectionPtr& conn,json& js,Timestamp time);
    //创建群组事件
    void createGroup(const TcpConnectionPtr& conn,json& js,Timestamp time);
    //群组添加好友事件
    void addGroup(const TcpConnectionPtr& conn,json& js,Timestamp time);
    //群组聊天事件
    void chatGroup(const TcpConnectionPtr& conn,json& js,Timestamp time);
    //退出聊天界面事件
    void loginout(const TcpConnectionPtr& conn,json& js,Timestamp time);

    //获取message中的消息id,以便通过id找到对应的事件处理函数
    MsgHandler getHandlet(int msgid);

    //处理客户端异常退出的问题，并且将其在线状态改为下线
    void clientCloseException(const TcpConnectionPtr& conn);

    //处理服务器利用ctrl+c退出时，导致用户的在线状态没有得到更改
    void reset();

     // 从redis消息队列中获取订阅的消息
    void handleRedisSubscribeMessage(int, string);

private:
    ChatService();//单例模式私有化
    //定义id对应的业务逻辑的实现
    unordered_map<int,MsgHandler>_msgHandlerMap;


    //以及在后续聊天服务器中，当用户1给用户2发送消息时，首先发送给服务器
    //然后由服务器将消息推送给用户2，所以要建立一个存储用户链接的结构
    //要记录对应用户的连接，否则如何找到用户连接呢？
    unordered_map<int,TcpConnectionPtr>_userConnMap;

    //由于_userConnMap会被多个线程调用，并且伴随着用户的上线，下线就会影响连接的状态
    //所以利用锁机制，进行多线程保护
    mutex _connMutex;

    //定义数据处理
    UserModel _usermodel;

    //离线消息数据对象
    OfflineMsgModel _offlineMsgModel;
    //添加好友的数据对象
    FriendModel _friendModel;
    //群组的业务对象
    GroupModel _groupModel;

    //定义redis对象
    Redis _redis;

};






#endif