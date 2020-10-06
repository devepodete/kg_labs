cmake_minimum_required(VERSION 3.17)
project(kg)

set(CMAKE_CXX_STANDARD 17)

add_executable(kg main.cpp camera.h keyLogger.h)

find_library(GLFW_LIB glfw)
find_library(VULKAN_LIB vulkan)
target_link_libraries(kg PRIVATE "${GLFW_LIB}")
target_link_libraries(kg PRIVATE "${VULKAN_LIB}")
