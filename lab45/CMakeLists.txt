cmake_minimum_required(VERSION 3.17)
project(kg45)

set(CMAKE_CXX_STANDARD 17)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SOURCES "${SRC_DIR}/main.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

set(GLFW_DIR "/usr/include/GLFW")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
target_link_libraries(${PROJECT_NAME} "glfw" ${GLFW_LIBRARIES})
target_include_directories(${PROJECT_NAME} PRIVATE ${GLFW_DIR})
target_compile_definitions(${PROJECT_NAME} PRIVATE "GLFW_INCLUDE_NONE")


set(GLAD_DIR "/usr/include/glad")
add_library("glad" "${GLAD_DIR}/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}")
target_link_libraries(${PROJECT_NAME} "glad" ${CMAKE_DL_LIBS})