//对user类进行操作的类

#ifndef USERMODEL_H
#define USERMODEL_H

#include"user.hpp"

//这里是对user表的数据库操作，增删查改等

class UserModel
{
public:
    //增加用户信息
    bool insert(User& user);

    //进行查询操作，查询用户信息
    User query(int id);
    
    bool updateState(User& user);

    //处理服务器利用ctrl+c退出时，导致用户的在线状态没有得到更改
    void resetState();

private:

};

#endif