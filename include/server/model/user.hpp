#ifndef USER_H
#define USER_H

#include<string>
using namespace std;

//根据对应的表，建立对应的处理

class User
{
public:
    User(int id=-1,string na="",string pa="",string st="offline")
    {
        this->id=id;
        this->name=na;
        this->password=pa;
        this->state=st;
    }

    //设置登录信息
    void setId(int id){this->id=id;}
    void setName(string na){this->name=na;}
    void setPass(string pass){this->password=pass;}
    void setState(string state){this->state=state;}

    //返回用户信息
    int getId(){return this->id;}
    string getName(){return this->name;}
    string getPass(){return this->password;}
    string getState(){return this->state;}

protected:
    int id;
    string name;
    string password;
    string state;
};



#endif