#ifndef FRIENDMODEL_H
#define FRIENDMODEL_H

#include"user.hpp"
#include"db.h"
#include<vector>
using namespace std;


class FriendModel
{
public:
    //插入好友信息，添加到friend表中
    void insert(int userid,int firendid);

    //返回查询到的好友列表
    vector<User> query(int userid);
};

#endif