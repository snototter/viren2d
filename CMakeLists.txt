cmake_minimum_required(VERSION 3.12)
project(vivi)

# ./cmake includes custom Find<...> modules, e.g. for Cairo
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 14)

#########################################################################################
# C++ library

# Header files
## Currently, all should be shipped as the public library interface
set(VIVI_HEADER_FILES
    include/vivi/colors.hpp
    include/vivi/drawing.hpp
    include/vivi/math.hpp
    include/vivi/primitives.hpp
    include/vivi/string_utils.hpp
    include/vivi/vivi.hpp
)

# Source files
## C++ library only
set(VIVI_SOURCE_FILES
    src/vivi/primitives.cpp
    src/vivi/colors.cpp
    src/vivi/drawing.cpp
    src/vivi/drawing_helpers.hpp
    src/vivi/drawing_helpers_primitives.cpp
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
    ${VIVI_HEADER_FILES}
    ${VIVI_SOURCE_FILES}
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
pybind11_add_module(vivi
    ${VIVI_HEADER_FILES}
    ${VIVI_SOURCE_FILES}
    src/vivi/bindings.cpp
)

target_link_libraries(vivi PRIVATE ${Cairo_LIBS} ${OpenCV_LIBRARIES})

target_compile_definitions(vivi
                           PRIVATE VERSION_INFO=${PYVIVI_VERSION_INFO})

