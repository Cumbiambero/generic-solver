cmake_minimum_required(VERSION 3.22)

project(solver)

set(CMAKE_CXX_STANDARD 23)
set(SOURCES src/main.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})
add_subdirectory(test)