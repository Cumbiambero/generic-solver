cmake_minimum_required(VERSION 3.22)

project(solver)

set(CMAKE_CXX_STANDARD 23)
set(SOURCES src/main.cpp)

option(BUILD_TESTING "Build tests" OFF)

if(BUILD_TESTING)
  find_package(doctest CONFIG)
  if(NOT doctest_FOUND)
    message(WARNING "doctest not found! Please install doctest (e.g., 'sudo apt-get install doctest-dev' or 'vcpkg install doctest').")
  endif()
  add_subdirectory(test)
endif()

add_executable(${PROJECT_NAME} ${SOURCES})