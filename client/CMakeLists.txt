cmake_minimum_required(VERSION 3.16)
project(client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

add_executable(client
    src/main.cpp
    src/client.cpp
)

target_include_directories(client PRIVATE include)
