cmake_minimum_required(VERSION 3.14)
# We need at least CMake 3.14 for FetchContent_MakeAvailable()

project(viren2d-demo-hello-world)


# viren2d requires at least C++14
set(CMAKE_CXX_STANDARD 14)


# Let CMake download and set up the viren2d dependency
include(FetchContent)
FetchContent_Declare(
  viren2d
  GIT_REPOSITORY https://github.com/snototter/viren2d.git
  GIT_TAG main)
FetchContent_MakeAvailable(viren2d)


# Create the demo executable
add_executable(hello-world
    ${CMAKE_CURRENT_LIST_DIR}/hello_world.cpp)

# Add the viren2d dependency (include paths will be set up automatically)
target_link_libraries(hello-world PRIVATE viren2d++::viren2d++)


# Optionally add OpenCV to display images:
find_package(OpenCV QUIET)
if(${OpenCV_FOUND})
    message(STATUS "The hello-world demo will use OpenCV (core + highgui module) to display images.")

    target_compile_definitions(hello-world
        PRIVATE WITH_OPENCV)

    target_link_libraries(hello-world
        PRIVATE ${OpenCV_LIBRARIES})
endif()

