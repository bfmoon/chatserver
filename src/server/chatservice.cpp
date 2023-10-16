#include"public.hpp"
#include"chatservice.hpp"
#include"user.hpp"
#include<muduo/base/Logging.h>
#include<vector>
#include<iostream>

using namespace std;

//返回单例对象
ChatService* ChatService::instance()
{
    static ChatService service;
    return &service;
}

//构造函数，初始化成员
ChatService::ChatService()
{
    //将枚举类型对应的事件处理函数放入map中
    _msgHandlerMap.insert({LOGIN_MSG,std::bind(&ChatService::login,this,_1,_2,_3)});

    _msgHandlerMap.insert({REG_MSG,std::bind(&ChatService::reg,this,_1,_2,_3)});

    //放入一对一聊天事件
    _msgHandlerMap.insert({ONE_CHAT_MSG,std::bind(&ChatService::oneChat,this,_1,_2,_3)});

    //添加好友的事件
    _msgHandlerMap.insert({ADD_FRIEND_MSG,std::bind(&ChatService::addFriend,this,_1,_2,_3)});

    //创建群组事件的处理
    _msgHandlerMap.insert({CREATE_GROUP_MSG,std::bind(&ChatService::createGroup,this,_1,_2,_3)});
    //群组中添加好友的事件处理
    _msgHandlerMap.insert({ADD_GROUP_MSG,std::bind(&ChatService::addGroup,this,_1,_2,_3)});
    //群组聊天的事件处理
    _msgHandlerMap.insert({GROUP_CHAT_MSG,std::bind(&ChatService::chatGroup,this,_1,_2,_3)});
    //加入退出聊天界面的事件
    _msgHandlerMap.insert({LOGINOUT_MSG,std::bind(&ChatService::loginout,this,_1,_2,_3)});

    // 连接redis服务器
    if (_redis.connect())
    {
        cout<<"connect redis "<<endl;
        // 设置上报消息的回调
        _redis.init_notify_handler(std::bind(&ChatService::handleRedisSubscribeMessage, this, _1, _2));
    }

}

//处理服务器利用ctrl+c退出时，导致用户的在线状态没有得到更改
void ChatService::reset()
{
    _usermodel.resetState();
}

//返回根据网络消息发送的id对应的事件处理函数，若没有则返回一个空实现，并打印日志
MsgHandler ChatService::getHandlet(int msgid)
{
    auto it=_msgHandlerMap.find(msgid);
    //说明没找到
    if(it==_msgHandlerMap.end())
    {
        //返回空的任务处理即可
        return [=](const TcpConnectionPtr& conn,json& js,Timestamp time) {
            LOG_ERROR<<"msgId"<<msgid<<"can't find handler!";
        };
    }
    else
    {
        return _msgHandlerMap[msgid];
    }
}

//处理登录的业务逻辑
void ChatService::login(const TcpConnectionPtr& conn,json& js,Timestamp time)
{
    //利用账号进行登录
    int id=js["id"];
    string pwd=js["password"];


    User user=_usermodel.query(id);

    if(user.getId()==id && pwd==user.getPass())
    {
        
        if(user.getState()=="online")
        {
            //表示用户已经在线，无法登录
            json response;

            response["msgid"] = LOGIN_MSG_ACK;
            // 注册成功错误码为0，注册失败错误码为1
            response["errno"] = 2;
            response["errmsg"]="该账号已经登录，请重新输入账号";

            // 然后将结果发送给客户端
            conn->send(response.dump());
        }
        //表示登录成功
        else
        {

            //这里登录成功之后，就记录对应连接
            //注意，该map是被多个线程都要进行调用的，都要记录每个用户的连接
            //所以要注意线程安全问题
            {
                lock_guard<mutex>lock(_connMutex);
                _userConnMap.insert({id,conn});
            }
            
            // id用户登录成功后，向redis订阅channel(id)
            _redis.subscribe(id); 
            cout<<"connect redis "<<endl;
            //则表示用户当前登录，修改登录状态为在线
            user.setState("online");

            _usermodel.updateState(user);

            json response;

            response["msgid"] = LOGIN_MSG_ACK;
            // 登录成功错误码为0，
            response["errno"] = 0;

            response["id"] = user.getId();
            response["name"]=user.getName();

            //登录成功，查看是否有离线消息
            vector<string>vec=_offlineMsgModel.query(id);
            if(!vec.empty())
            {
                response["msg"]=vec;
                //读取用户登录之后，删除所有存储的离线消息
                _offlineMsgModel.remove(id);
            }

            //登录成功之后，要给用户展示好友列表
            vector<User>userVec=_friendModel.query(id);
            if(!userVec.empty())
            {
                //将该用户的好友信息添加到列表中
                vector<string>friendInfo;
                for(User& user:userVec)
                {
                    json js;
                    js["id"]=user.getId();
                    js["name"]=user.getName();
                    js["state"]=user.getState();
                    friendInfo.push_back(js.dump());
                }
                response["friend"]=friendInfo;
            }
            

            //登录成功，给用户展示群组列表
            vector<Group>groupVec=_groupModel.queryGroups(id);
            if(!groupVec.empty())
            {
               
                //将群组信息添加到群组列表中
                vector<string>groupInfo;
                for(Group& group:groupVec)
                {
                    json js;
                    js["id"]=group.getId();
                    js["groupname"]=group.getName();
                    js["groupdesc"]=group.getDesc();
                    
                    //还有群组成员
                    vector<string>user;
                    for(GroupUser& groupuser:group.getUser())
                    {
                        json userjs;
                        userjs["id"]=groupuser.getId();
                        userjs["name"]=groupuser.getName();
                        userjs["state"]=groupuser.getState();
                        userjs["role"]=groupuser.getRole();
                        user.push_back(userjs.dump());
                    }
                    js["users"]=user;
                    groupInfo.push_back(js.dump());
                }

                response["groups"]=groupInfo;
            }
            // 然后将结果发送给客户端
            conn->send(response.dump());
        }

    }

    else
    {
        //否则表示登录失败
        json response;
        response["msgid"]=LOGIN_MSG_ACK;
        //注册成功错误码为0，注册失败错误码为1
        response["errno"]=1;
        response["errmsg"]="账号或密码错误，请重新登录！";
        //然后将结果发送给客户端
         conn->send(response.dump());

    }
    
}

//处理注册事件的业务逻辑
void ChatService::reg(const TcpConnectionPtr& conn,json& js,Timestamp time)
{
    //处理注册事件
    //包括姓名和密码，从js中获取
    string name=js["name"];

    string password=js["password"];

    User user;
    user.setName(name);
    user.setPass(password);

    bool state=_usermodel.insert(user);
    if(state)
    {
        //说明注册成功
        //要进行消息返回
        json response;
        response["msgid"]=REG_MSG_ACK;
        //注册成功错误码为0，注册失败错误码为1
        response["errno"]=0;

        response["id"]=user.getId();

        //然后将结果发送给客户端
         conn->send(response.dump());
    }
    else
    {
        //表示注册失败
        json response;
        response["msgid"]=REG_MSG_ACK;
        //注册成功错误码为1，注册失败错误码为0
        response["errno"]=1;

        //然后将结果发送给客户端
        conn->send(response.dump());
    }

}

//处理客户端异常退出的问题，并且将其在线状态改为下线
void ChatService::clientCloseException(const TcpConnectionPtr& conn)
{
    User user;
    //由于涉及多线程问题，并且若其他用户正在聊天，而该用户退出，受到影响
    //所以就要进行加锁操作
    
    {
        lock_guard<mutex> lock(_connMutex);
        // 然后查找对应id的用户连接
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); it++)
        {
            if (it->second == conn)
            {
                user.setId(it->first);
                // 说明找到了，进行删除操作
                _userConnMap.erase(it->first);

                break;
            }
        }
    }

    // 用户注销，相当于就是下线，在redis中取消订阅通道
    _redis.unsubscribe(user.getId()); 

    //然后修改其登录状态
    if(user.getId()!=-1)
    {
        user.setState("offline");
        _usermodel.updateState(user);
    }
}

//退出聊天界面事件
void ChatService::loginout(const TcpConnectionPtr& conn,json& js,Timestamp time)
{
    int userid=js["id"].get<int>();

    {
        lock_guard<mutex>lock(_connMutex);
        //查找要退出的id
        auto it=_userConnMap.find(userid);
        if(it!=_userConnMap.end())
        {
            _userConnMap.erase(userid);
        }
    }
        // 用户注销，相当于就是下线，在redis中取消订阅通道
    _redis.unsubscribe(userid); 

    User user(userid,"","","offline");
    _usermodel.updateState(user);
}

//一对一聊天事件
void ChatService::oneChat(const TcpConnectionPtr& conn,json& js,Timestamp time)
{
    int toId=js["to"].get<int>();

    //然后在map中查找toId对应的用户是否在连接表里面
    //若在,则表示在线，否则表示离线
    //查表操作，所以要进行加锁
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toId);
        if (it != _userConnMap.end())
        {
            // 表示用户在线，即可发送消息
            it->second->send(js.dump());
            return;
        }
    }
    // 查询toid是否在线 
    User user =_usermodel.query(toId);
    if (user.getState() == "online")
    {
        _redis.publish(toId, js.dump());
        return;
    }

    //否则就是不在线，服务器进行消息缓存，存在离线消息表中
    _offlineMsgModel.insert(toId,js.dump());
}

//添加好友事件
void ChatService::addFriend(const TcpConnectionPtr& conn,json& js,Timestamp time)
{
    int userid=js["id"].get<int>();
    int friendid=js["friendid"].get<int>();

    //添加好友信息
    _friendModel.insert(userid,friendid);
}

//创建群组的事件
void ChatService::createGroup(const TcpConnectionPtr& conn,json& js,Timestamp time)
{
    //获取客户端发送的json数据
    int userid=js["id"].get<int>();
    string name=js["groupname"];
    string desc=js["groupdesc"];

    //封装群对象，由于群id是主键自增长
    //并且现在并创建群，所以id为-1
    Group group(-1,name,desc);
    //然后调用创建群的函数，通过mysql中的函数来获取创建成功后的群id
    //并且将其赋值
    if(_groupModel.createGroup(group))
    {
        //说明创建成功，添加创建人的信息，在群组用户表中
        _groupModel.addGroup(userid,group.getId(),"creator");
    }
}

// 群组添加好友事件
void ChatService::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid=js["id"].get<int>();
    int groupid=js["groupid"].get<int>();

    _groupModel.addGroup(userid,groupid,"normal");
}


// 群组聊天事件
void ChatService::chatGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid=js["id"].get<int>();
    int groupid=js["groupid"].get<int>();

    //由于是群聊，所以要获取其他用户id，然后进行聊天
    vector<int>userVec=_groupModel.queryGroupUsers(userid,groupid);

    lock_guard<mutex>lock(_connMutex);
    for(int id:userVec)
    {
        //然后通过这些id在之前记录的用户连接表中查找
        //若有，说明在线，就可以直接发送，若没有，说明离线，则进行消息缓存
        auto it=_userConnMap.find(id);
        if(it!=_userConnMap.end())
        {
            it->second->send(js.dump());
        }
        else
        {
            // 查询toid是否在线 
            User user = _usermodel.query(id);
            if (user.getState() == "online")
            {
                _redis.publish(id, js.dump());
            }
            else
            {
                // 存储离线群消息
                _offlineMsgModel.insert(id, js.dump());
            }
        }
    }
}

// 从redis消息队列中获取订阅的消息
void ChatService::handleRedisSubscribeMessage(int userid, string msg)
{
    lock_guard<mutex> lock(_connMutex);
    auto it = _userConnMap.find(userid);
    if (it != _userConnMap.end())
    {
        it->second->send(msg);
        return;
    }

    // 存储该用户的离线消息
    _offlineMsgModel.insert(userid, msg);
}