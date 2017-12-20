cmake_minimum_required(VERSION 3.9)
project(SocksEvPractise C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lev")

add_executable(SocksEvPractise main.c common.h server.c server.h main.h data_list.c data_list.h)
target_link_libraries(SocksEvPractise ev)