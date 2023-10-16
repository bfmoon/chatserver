#ifndef GROUP_H
#define GROUP_H

#include"groupuser.hpp"
#include<string>
#include<vector>
using namespace std;


//群组表的相关属性

class Group
{
public:
    Group(int id=-1,string name="",string desc="")
    {
        this->id=id;
        this->name=name;
        this->desc=desc;
    }

    //设置群组表的相关属性
    void setId(int id){this->id=id;}
    void setName(string name){this->name=name;}
    void setDesc(string desc){this->desc=desc;}

    int  getId(){return this->id;}
    string getName(){return this->name;}
    string getDesc(){return this->desc;}
    vector<GroupUser>& getUser(){return this->user;}

private:
    int id;//群组的id号
    string name;//群组名
    string desc;//群组的介绍
    vector<GroupUser>user;//记录所有群组用户信息

};



#endif