#include"friendmodel.hpp"

//插入好友信息，添加到friend表中
void FriendModel::insert(int userid,int firendid)
{
    char sql[1024]={0};

    sprintf(sql,"insert into friend values(%d,%d)",userid,firendid);

    //memcpy(sql,"insert into user(name,password,state) values('zhang san','123445','offline')",77);
    MySQL mysql;
    if(mysql.connect())
    {
        mysql.update(sql);
    }

}

//返回查询到的好友列表
vector<User> FriendModel::query(int userid)
{
    char sql[1024]={0};

    //利用多表查询，将userid用户的朋友列表展示

    sprintf(sql,"select a.id,a.name,a.state from user a inner join friend b on b.friendid= a.id where b.userid=%d",
            userid);
    
    vector<User>vec;

    MySQL mysql;
    if(mysql.connect())
    {
        //进行查询操作时，将查询到的结果集返回
        MYSQL_RES* res=mysql.query(sql);

        if(res!=nullptr)
        {
            //获取结果集中的每一列值
            MYSQL_ROW row;
            while((row= mysql_fetch_row(res))!=nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);

                vec.push_back(user);
            }
            mysql_free_result(res);
            return vec;
        }
    }
    return vec;
}