#include"usermodel.hpp"
#include"db.h"
#include<string>
#include<iostream>
#include<stdio.h>
using namespace std;

bool UserModel::insert(User& user)
{
    char sql[1024]={0};

    sprintf(sql,"insert into user(name,password,state) values('%s','%s','%s')",
    user.getName().c_str(),user.getPass().c_str(),user.getState().c_str());

    //memcpy(sql,"insert into user(name,password,state) values('zhang san','123445','offline')",77);
    MySQL mysql;
    if(mysql.connect())
    {
        if(mysql.update(sql))
        {
            user.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    return false;
}

User UserModel::query(int id)
{
    char sql[1024]={0};

    //登录操作时，查询表中对应的id
    //若存在，则将表中值返回，否则返回默认值
    sprintf(sql,"select * from user where id=%d",id);
    
    MySQL mysql;
    if(mysql.connect())
    {
        //进行查询操作时，将查询到的结果集返回
        MYSQL_RES* res=mysql.query(sql);

        if(res!=nullptr)
        {
            //获取结果集中的每一列值
            MYSQL_ROW row = mysql_fetch_row(res);
            if(row!=nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setPass(row[2]);
                user.setState(row[3]);
                // 释放查询到的结果集
                mysql_free_result(res);
                return user;
            }  
        }
    }
    //由于User类中有默认值，id默认为-1，可以依旧此判断是否登录成功
    return User();

}


//更新用户登录状态的操作
bool UserModel::updateState(User& user)
{
    char sql[1024]={0};

    sprintf(sql,"update user set state='%s' where id=%d",user.getState().c_str(),user.getId());

    MySQL mysql;
    if(mysql.connect())
    {
        if(mysql.update(sql))
        {
            return true;
        }
    }
    return false;
}

//处理服务器利用ctrl+c退出时，导致用户的在线状态没有得到更改
void UserModel::resetState()
{
    char sql[1024]="update user set state='offline' where state='online'";

    MySQL mysql;
    if(mysql.connect())
    {
        mysql.update(sql);
    }
}