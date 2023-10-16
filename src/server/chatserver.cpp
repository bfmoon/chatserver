#include "chatserver.hpp"
#include"chatservice.hpp"
#include"json.hpp"

#include<functional>
#include<string>

using namespace std;
using namespace placeholders;
using json=nlohmann::json;

ChatServer::ChatServer(EventLoop *loop, const InetAddress &listenAddr, const string serverName)
    : _server(loop, listenAddr, serverName), _loop(loop)
{
    //设置事件回调函数
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection,this,_1));

    //设置消息的事件回调
    _server.setMessageCallback(std::bind(&ChatServer::onMessage,this,_1,_2,_3));

    //开启4个线程
    _server.setThreadNum(4);

}

void ChatServer::start()
{
    _server.start();
}
void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    //如果没有连接成功，则直接退出
    if(!conn->connected())
    {
        //这里处理客户端退出的情况，并且将其的在线状态改为下线
        ChatService::instance()->clientCloseException(conn);

        conn->shutdown();
    }
}

//处理消息的事件函数
void ChatServer::onMessage(const TcpConnectionPtr& conn
            ,muduo::net::Buffer* buffer
            ,muduo::Timestamp time)
{
    //将收到的消息，转为字符串
    string buf=buffer->retrieveAllAsString();
    //然后将得到的消息进行反序列化,就是将字符串进行解码

    //该解码中有通过id指向的某种消息类型，例如[1,“登录”],[2,"注册"]等操作
    //实现一个id对应一个事件处理函数，如1对应登录的要求，则解析出1,则直接调用登录函数即可
    
    //但是如果通过if语句进行判断时，就会产生很多可能且不方便增加，耦合性太差
    //所以定义一个消息服务类，专门处理某种消息
    json js=json::parse(buf);

    //这里通过单例模式调用业务类中获取id对应事务处理的方法
    auto msgHandler=ChatService::instance()->getHandlet(js["msgid"].get<int>());
    //然后直接执行该函数
    msgHandler(conn,js,time);


}