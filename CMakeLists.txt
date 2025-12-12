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

file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS
    src/solver/*.cpp
    src/solver/*.cxx
    src/solver/*.cc
    src/tree/*.cpp
    src/tree/*.cxx
    src/tree/*.cc
    src/utils/*.cpp
    src/utils/*.cxx
    src/utils/*.cc
)
list(APPEND CORE_SOURCES src/lib_wrapper.cpp)
list(APPEND CORE_SOURCES src/solver_core_wrapper.cpp)
set(SOURCES src/main.cpp)

option(BUILD_SHARED_LIB "Build solver core as shared library" ON)

if(BUILD_SHARED_LIB)
  add_library(solver_core SHARED ${CORE_SOURCES})
else()
  add_library(solver_core STATIC ${CORE_SOURCES})
endif()

target_include_directories(solver_core PUBLIC
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)


option(BUILD_TESTING "Build tests" OFF)

if(BUILD_TESTING)
  enable_testing()
  find_package(doctest CONFIG)
  if(NOT doctest_FOUND)
    message(WARNING "doctest not found! Please install doctest (e.g., 'sudo apt-get install doctest-dev' or 'vcpkg install doctest').")
  endif()
  add_subdirectory(test)
endif()

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE solver_core)

# Link threads library
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

# Set target properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Installation and export
include(CMakePackageConfigHelpers)
set(INSTALL_LIB_DIR lib)
set(INSTALL_INC_DIR include)

install(TARGETS solver_core
  EXPORT solver_coreTargets
  LIBRARY DESTINATION ${INSTALL_LIB_DIR}
  ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
  RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION ${INSTALL_INC_DIR})

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/solver_coreConfigVersion.cmake"
  VERSION 1.0.0
  COMPATIBILITY AnyNewerVersion
)

install(EXPORT solver_coreTargets
  FILE solver_coreTargets.cmake
  NAMESPACE solver_core::
  DESTINATION ${INSTALL_LIB_DIR}/cmake/solver_core)

configure_file("${CMAKE_CURRENT_BINARY_DIR}/solver_coreConfigVersion.cmake" "${CMAKE_CURRENT_BINARY_DIR}/solver_coreConfig.cmake" COPYONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/solver_coreConfig.cmake"
  DESTINATION ${INSTALL_LIB_DIR}/cmake/solver_core)
