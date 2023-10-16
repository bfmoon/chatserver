#查找当前路径下的所有源文件
aux_source_directory(. SRC_LIST)

#指定生成可执行文件，以及依赖的源代码
add_executable(ChatClient ${SRC_LIST})

#指定可执行文件链接时需要依赖的库文件
target_link_libraries(ChatClient pthread)