cmake_minimum_required(VERSION 3.15)
set (CMAKE_CXX_STANDARD 14)

project(test)
include_directories(../vendor ../include ../../protocol/cpp)
add_executable(test src/main.cc)
