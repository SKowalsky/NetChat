cmake_minimum_required(VERSION 3.12)
project(NetChat)

set(CMAKE_CXX_STANDARD 17)

add_executable(NetChat main.cpp NetLib/UdpServer.cpp NetLib/UdpServer.h NetLib/UdpClient.cpp NetLib/UdpClient.h NetLib/Connection.h ChatServer.cpp ChatServer.h)