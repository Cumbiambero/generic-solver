cmake_minimum_required(VERSION 3.22)

project(solver LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable optimizations for Release builds
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -fsanitize=address,undefined")

# Enable more warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion")

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

# Link threads library
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

# Set target properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)