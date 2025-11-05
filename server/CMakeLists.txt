cmake_minimum_required(VERSION 3.16)
project(server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

add_executable(server
    src/main.cpp
    src/server.cpp
)

target_include_directories(server PRIVATE include)
