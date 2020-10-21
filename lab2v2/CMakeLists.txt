cmake_minimum_required(VERSION 3.17)
project(new_kg2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined")

add_executable(new_kg2 main.cpp matrices.h)
