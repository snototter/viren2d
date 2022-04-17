cmake_minimum_required(VERSION 3.12)
project(viren2d)

#TODO maybe rename cpp standalone library to viren2d++ ? (would make a nice .deb)

# ./cmake includes custom Find<...> modules, e.g. for Cairo
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 14)

#########################################################################################
# C++ library

# Header files
## Currently, all should be shipped as the public library interface
set(viren2d_PUBLIC_HEADER_FILES
    include/viren2d/colors.hpp
    include/viren2d/drawing.hpp
    include/viren2d/math.hpp
    include/viren2d/primitives.hpp
    include/viren2d/string_utils.hpp
    include/viren2d/viren2d.hpp
)

set(viren2d_PRIVATE_HEADER_FILES
    src/viren2d/drawing_helpers.hpp
)


# Source files
## C++ library only
set(viren2d_SOURCE_FILES
    src/viren2d/primitives.cpp
    src/viren2d/colors.cpp
    src/viren2d/drawing.cpp
    src/viren2d/drawing_helpers_primitives.cpp
)


# Include the library headers for all targets
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

# We use the header-only stb library for image data handling
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libs/stb")


# Uses the custom ./cmake/FindCairo.cmake
find_package(Cairo REQUIRED)
include_directories(${Cairo_INCLUDE_DIRS})

#TODO maybe needed when we start working on camera calibration, 3d stuff
#find_package(Eigen3 REQUIRED)
#include_directories(${EIGEN3_INCLUDE_DIR})


#########################################################################################
# C++ demo/examples

#TODO make library, then include library in demo
#TODO post config/build step - copy image/other assets to build folder
add_executable(demo2d
    ${viren2d_PUBLIC_HEADER_FILES}
    ${viren2d_PRIVATE_HEADER_FILES}
    ${viren2d_SOURCE_FILES}
    examples/demo2d.cpp
)
# TODO When we have a standalone cpp library, it expose/link/... Cairo_LIBS. Then, the following is no longer needed!
target_link_libraries(demo2d PRIVATE ${Cairo_LIBS})

#TODO check interface/public/private https://stackoverflow.com/a/49538748/400948
#TODO read https://kubasejdak.com/modern-cmake-is-like-inheritance
# OpenCV is optional, we only need it for displaying the rendered images
find_package(OpenCV)
if(${OpenCV_FOUND})
    add_definitions(-DWITH_OPENCV)
    target_include_directories(demo2d PRIVATE ${OpenCV_INCLUDE_DIRS})
    target_link_libraries(demo2d PRIVATE ${OpenCV_LIBRARIES})
endif()


#########################################################################################
# Python bindings

# pybind11 provides add_module which creates a target
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/pybind11)
pybind11_add_module(viren2d
    ${viren2d_PUBLIC_HEADER_FILES}
    ${viren2d_PRIVATE_HEADER_FILES}
    ${viren2d_SOURCE_FILES}
    src/viren2d/bindings.cpp
)

#TODO make cairo public??? check interface visibility!
target_link_libraries(viren2d PRIVATE ${Cairo_LIBS})

target_compile_definitions(viren2d
                           PRIVATE VERSION_INFO=${PYVIREN2D_VERSION_INFO})

