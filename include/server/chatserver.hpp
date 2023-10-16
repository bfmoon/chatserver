#ifndef CHATSERVER_H
#define CHATSERVER_H

#include<muduo/net/TcpServer.h>
#include<muduo/net/EventLoop.h>

using namespace muduo;
using namespace muduo::net;


//网络通信的服务

//初始化聊天服务器

class ChatServer
{
public:
    ChatServer(EventLoop* loop,const InetAddress& listenAddr,const string serverName);

    //开启
    void start();

private:
    //定义事件回调函数
    

    //处理连接事件的回调
    void onConnection(const TcpConnectionPtr& conn);


    //处理收发消息的事件回调函数
    void onMessage(const TcpConnectionPtr& conn
            ,muduo::net::Buffer* buffer
            ,muduo::Timestamp time );
private:
    //定义服务对象
    TcpServer _server;
    //定义事件循环对象
    EventLoop *_loop;
};






#endif