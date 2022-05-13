cmake_minimum_required(VERSION 3.14)
# We need at least CMake 3.14 for FetchContent_MakeAvailable()

project(viren2d-demo)


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
set(viren2d_WITH_PYTHON OFF CACHE BOOL "" FORCE) #TODO add text
set(viren2d_WITH_TESTS OFF CACHE BOOL "" FORCE)
set(viren2d_WITH_DEMO OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(viren2d)


# Create the demo executable
add_executable(demo
    ${CMAKE_CURRENT_LIST_DIR}/../demo2d.cpp)

# Add the viren2d dependency (include paths will be set up automatically)
target_link_libraries(demo PRIVATE viren2d++::viren2d++)

# We need to specify the path to an exemplary image
target_compile_definitions(demo PRIVATE
        EXAMPLE_IMAGE_FILE="${CMAKE_CURRENT_SOURCE_DIR}/../flamingo.jpg")

# Optionally add OpenCV to display images:
find_package(OpenCV QUIET)
if(${OpenCV_FOUND})
    message(STATUS "The demo will use OpenCV (core + highgui module) to display images.")

    target_compile_definitions(demo
        PRIVATE WITH_OPENCV)

    target_link_libraries(demo
        PRIVATE ${OpenCV_LIBRARIES})
endif()

