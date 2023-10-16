#include"groupmodel.hpp"
#include"db.h"
#include<iostream>

// 创建群组,就是在allgroup表中，加入一个群组的信息
bool GroupModel::createGroup(Group &group)
{
    char sql[1024]={0};

    sprintf(sql,"insert into allgroup(groupname,groupdesc) values('%s','%s')",
    group.getName().c_str(),group.getDesc().c_str());

    //memcpy(sql,"insert into user(name,password,state) values('zhang san','123445','offline')",77);
    MySQL mysql;
    if(mysql.connect())
    {
        if(mysql.update(sql))
        {
            group.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    return false;
}

// 添加群组成员,就是将一个用户添加到groupuser表中
// 用户id,群组id,进群的角色
void GroupModel::addGroup(int userid, int groupid, string role)
{
    char sql[1024]={0};

    sprintf(sql,"insert into groupuser(groupid,userid,grouprole) values(%d,%d,'%s')",groupid,userid,role.c_str());

    //memcpy(sql,"insert into user(name,password,state) values('zhang san','123445','offline')",77);
    MySQL mysql;
    if(mysql.connect())
    {
        mysql.update(sql);
    }
}

// 查询用户所在的群组信息,以及群组中的用户信息
vector<Group> GroupModel::queryGroups(int userid)
{
    char sql[1024] = {0};
    

    // 查询用户所在的表，要进行多表查询
    // 返回群组id,群组name,群组信息
    sprintf(sql, "select a.id,a.groupname,a.groupdesc from allgroup a inner join \
            groupuser b on a.id=b.groupid where b.userid=%d",
            userid);

    vector<Group> groupVec;

    MySQL mysql;
    if (mysql.connect())
    {
        // 进行查询操作时，将查询到的结果集返回
        MYSQL_RES *res = mysql.query(sql);

        if (res != nullptr)
        {
            // 获取结果集中的每一列值
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                Group group;
                group.setId(atoi(row[0]));
                group.setName(row[1]);
                group.setDesc(row[2]);
                groupVec.push_back(group);
                
            }
            // 释放查询到的结果集
            mysql_free_result(res);
        }
    }

    // 第2步，查看群组中的用户信息
    for (Group &group : groupVec)
    {
        sprintf(sql, "select a.id,a.name,a.state,b.grouprole from user a inner join \
            groupuser b on b.userid=a.id where b.groupid=%d",
                group.getId());

        // 进行查询操作时，将查询到的结果集返回
        MYSQL_RES *res = mysql.query(sql);

        if (res != nullptr)
        {
            // 获取结果集中的每一列值
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                
                GroupUser user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                user.setRole(row[3]);
                std::cout<<user.getId()<<endl;
                group.getUser().push_back(user);
            }
            std::cout<<group.getUser().size()<<std::endl;
            // 释放查询到的结果集
            mysql_free_result(res);
        }
    }
    return groupVec;
}


// 群组聊天功能
//  根据指定的groupid查询群组用户id列表，除userid自己，主要用户群聊业务给群组其它成员群发消息
vector<int> GroupModel::queryGroupUsers(int userid, int groupid)
{
    char sql[1024] = {0};
    sprintf(sql, "select userid from groupuser where groupid = %d and userid != %d", groupid, userid);

    vector<int> idVec;
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES *res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res)) != nullptr)
            {
                idVec.push_back(atoi(row[0]));
            }
            mysql_free_result(res);
        }
    }
    return idVec;
}