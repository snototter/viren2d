cmake_minimum_required(VERSION 3.14)
# We need at least CMake 3.14 for FetchContent_MakeAvailable()

project(viren2d-demo-external-fetch)


# viren2d requires at least C++14
set(CMAKE_CXX_STANDARD 14)


# Let CMake download and set up the viren2d dependency
include(FetchContent)
FetchContent_Declare(
  viren2d
  GIT_REPOSITORY https://github.com/snototter/viren2d.git
  GIT_TAG main
  GIT_SUBMODULES "libs/stb"
)
FetchContent_MakeAvailable(viren2d)


# Create the demo executable
add_executable(demo
    ${CMAKE_CURRENT_LIST_DIR}/../demo2d.cpp)

# Add the viren2d dependency (include paths will be set up automatically)
target_link_libraries(demo PRIVATE viren2d::viren2d++)

