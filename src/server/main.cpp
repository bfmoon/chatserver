#include"chatserver.hpp"
#include<iostream>
#include<signal.h>
#include"chatservice.hpp"

void resetHandler(int )
{
    ChatService::instance()->reset();
    exit(0);
}

int main(int argc,char**argv)
{
    if(argc<3)
    {
        cerr<<"commend invalid example:127.0.0.1 8000"<<endl;
        exit(-1);
    }
    char* ip=argv[1];
    uint16_t  port=atoi(argv[2]);
    //信号处理函数，处理服务器利用ctrl+c退出时，导致用户的在线状态没有得到更改
    signal(SIGINT,resetHandler);

    EventLoop loop;

    InetAddress addr(ip,port);

    ChatServer server(&loop,addr,"chatserver");

    server.start();

    //开启事件循环
    loop.loop();


    return 0;
}