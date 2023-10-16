#include"offlinemessageModel.hpp"
#include"db.h"

// 插入离线消息

void OfflineMsgModel::insert(int userid, string msg)
{
    char sql[1024]={0};

    sprintf(sql,"insert into offlinemessage values(%d,'%s')",userid,msg.c_str());

    //memcpy(sql,"insert into user(name,password,state) values('zhang san','123445','offline')",77);
    MySQL mysql;
    if(mysql.connect())
    {
        mysql.update(sql);
    }

}

// 当用户登录，接收到离线消息后就删除缓存的离线消息
void OfflineMsgModel::remove(int userid)
{
     char sql[1024]={0};

    sprintf(sql,"delete from offlinemessage  where userid=%d",userid);

    MySQL mysql;
    if(mysql.connect())
    {
        mysql.update(sql);
    }
       
}

// 查询用户的离线消息
vector<string> OfflineMsgModel::query(int userid)
{
    char sql[1024]={0};

    //登录操作时，查询表中对应的id
    //若存在，则将表中值返回，否则返回默认值
    sprintf(sql,"select * from offlinemessage where userid=%d",userid);
    
    vector<string>vec;

    MySQL mysql;
    if(mysql.connect())
    {
        //进行查询操作时，将查询到的结果集返回
        MYSQL_RES* res=mysql.query(sql);

        if(res!=nullptr)
        {
            //获取结果集中的每一列值
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res))!=nullptr)
            {
                vec.push_back(row[1]);
            }

            mysql_free_result(res);
            return vec;
        }
    }
    //由于User类中有默认值，id默认为-1，可以依旧此判断是否登录成功
    return vec;
}
