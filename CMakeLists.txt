cmake_minimum_required(VERSION 3.12)
project(vivi)

# ./cmake includes custom Find<...> modules, e.g. for Cairo
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_CXX_STANDARD 14)

#FIXME Ninja doesn't work with setup.py
#FIXME must be set as OS.env set(CMAKE_GENERATOR "Unix Makefiles")

set(VIVI_SOURCE_FILES
    src/vivi/primitives.cpp
    src/vivi/drawing.cpp
)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")


find_package(Cairo REQUIRED)
include_directories(${Cairo_INCLUDE_DIRS})

#find_package(Eigen3 REQUIRED)
#include_directories(${EIGEN3_INCLUDE_DIR})

#TODO make library, then include library in demo
#TODO post config/build step - copy image to build folder
add_executable(demo2d
    ${VIVI_SOURCE_FILES}
    examples/demo2d.cpp
)

find_package(OpenCV REQUIRED)
#TODO check interface/public/private https://stackoverflow.com/a/49538748/400948
#TODO read https://kubasejdak.com/modern-cmake-is-like-inheritance
target_include_directories(demo2d PRIVATE ${OpenCV_INCLUDE_DIRS})
#include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(demo2d PRIVATE ${Cairo_LIBS} ${OpenCV_LIBRARIES})

#message(INFO "${Cairo_LIBRARIES}")
#message(INFO "${Cairo_LIBS}")


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/pybind11)
pybind11_add_module(vivi
    ${VIVI_SOURCE_FILES}
    src/vivi/bindings.cpp)

target_link_libraries(vivi PRIVATE ${Cairo_LIBS} ${OpenCV_LIBRARIES})

target_compile_definitions(vivi
                           PRIVATE VERSION_INFO=${PYVIVI_VERSION_INFO})

