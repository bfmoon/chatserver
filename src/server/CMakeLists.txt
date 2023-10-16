

#定义一个变量，包含该目录下所有的源文件
aux_source_directory(. SRC_LIST)
aux_source_directory(./db DB_LIST)
aux_source_directory(./model MO_LIST)
aux_source_directory(./redis RE_LIST)

#指定生成可执行文件，以及依赖的源代码
add_executable(ChatServer ${SRC_LIST} ${DB_LIST} ${MO_LIST} ${RE_LIST})

#指定可执行文件链接时需要依赖的库文件
target_link_libraries(ChatServer muduo_net muduo_base mysqlclient hiredis pthread)

