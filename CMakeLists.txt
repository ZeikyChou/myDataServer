cmake_minimum_required(VERSION 3.10)
project(DataServer)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 添加宏定义


# 设置源代码目录和二进制文件目录
set(SRC_DIR ${PROJECT_SOURCE_DIR})
set(BIN_DIR ${PROJECT_SOURCE_DIR}/bin)

#包含头文件路径
include_directories(SRC_DIR)
# include_directories(${SRC_DIR}/RedisValue)




# 添加源文件
set(SOURCE_FILES 
    ${SRC_DIR}/CommandExecutor.cpp 
    ${SRC_DIR}/CommandParser.cpp 
    ${SRC_DIR}/CommandExecutor.hpp
    ${SRC_DIR}/CommandParser.hpp
    # ${SRC_DIR}/server.cpp 
)

# 确保二进制文件目录存在
file(MAKE_DIRECTORY ${BIN_DIR})

add_compile_options(-ggdb -g -O0 -Wall   -fstack-protector-all -march=native -fnon-call-exceptions)
# 编译server
add_executable(server ${SRC_DIR}/server.cpp ${SOURCE_FILES})
set_target_properties(server PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
# target_link_libraries(server zmq)

# 编译client
# add_executable(client ${SRC_DIR}/client.cpp)
# set_target_properties(client PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
# target_link_libraries(client zmq)