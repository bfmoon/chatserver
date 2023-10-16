#ifndef OFFLINEMESSAGEMODEL_H
#define OFFLINEMESSAGEMODEL_H
#include<string>
#include<vector>
using namespace std;



//缓存离线消息类

class OfflineMsgModel
{
public:
    //插入离线消息
    void insert(int userid,string msg);

    //当用户登录，接收到离线消息后就删除缓存的离线消息
    void remove(int userid);

    //查询用户的离线消息
    vector<string> query(int userid);

};

#endif