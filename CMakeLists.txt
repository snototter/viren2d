cmake_minimum_required(VERSION 3.14)

# Version could have been injected (e.g. by the CMakeBuild
# extension in setup.py)
if (NOT viren2d_VERSION)
  file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" viren2d_VERSION)
endif()

    
# Set up the project
project(viren2d VERSION ${viren2d_VERSION} LANGUAGES CXX)



#TODO disable before v1 release (except for install)
option(viren2d_WITH_DEMO "Build C++ demo." ON)
option(viren2d_WITH_PYTHON "Build Python bindings." ON)
option(viren2d_WITH_TESTS "Build test suite." ON)
option(INSTALL_viren2d "Configure installation target." ON)


# Offer the user the choice of overriding the installation (sub-)directories
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    # will not take effect without FORCE
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install/v${viren2d_VERSION}"
        CACHE PATH "Installation top-level directory" FORCE)
endif()
set(INSTALL_DIR_LIB lib CACHE PATH "Installation directory for library (relative to CMAKE_INSTALL_PREFIX or absolute)")
set(INSTALL_DIR_INCLUDE include CACHE PATH "Installation directory for header files (relative to CMAKE_INSTALL_PREFIX or absolute)")
set(INSTALL_DIR_CMAKE cmake CACHE PATH "Installation directory for CMake files (relative to CMAKE_INSTALL_PREFIX or absolute)")

# Ensure all paths are absolute (needed later on)
foreach(p LIB INCLUDE CMAKE)
    set(var INSTALL_DIR_${p})
    if(NOT IS_ABSOLUTE "${${var}}")
        set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
    endif()
endforeach()


# Set compiler options
set(CMAKE_CXX_STANDARD 14)  # We use std::exchange
set(CMAKE_CXX_EXTENSIONS OFF)  # only std c++11, not gnu++11
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")


# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()


# Compile library with -fPIC
set(CMAKE_POSITION_INDEPENDENT_CODE ON)



#########################################################################################
# C++ library

# Header files
set(viren2d_PUBLIC_HEADER_FILES
    include/viren2d/colors.hpp
    include/viren2d/drawing.hpp
    include/viren2d/math.hpp
    include/viren2d/primitives.hpp
    include/viren2d/string_utils.hpp  #TODO replace by cpputils::stringutils
    include/viren2d/viren2d.hpp)


set(viren2d_PRIVATE_HEADER_FILES
    src/drawing_helpers/drawing_helpers.hpp)


# Source files
set(viren2d_SOURCE_FILES
    src/primitives.cpp
    src/colors.cpp
    src/drawing.cpp
    src/drawing_helpers/drawing_helpers_primitives.cpp)


# Define the core library
add_library(viren2d
    ${viren2d_PUBLIC_HEADER_FILES}
    ${viren2d_PRIVATE_HEADER_FILES}
    ${viren2d_SOURCE_FILES})


# Add a namespace'd alias
add_library(viren2d::viren2d ALIAS viren2d)


# Set up the include paths for internal usage (PRIVATE), usage if this CMakeLists.txt is
# included in another project (PUBLIC BUILD_INTERFACE) or if the installed library is
# used (PUBLIC INSTALL_INTERFACE)
target_include_directories(${PROJECT_NAME}
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src"
            "${CMAKE_CURRENT_SOURCE_DIR}/libs/stb"
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:${INSTALL_DIR_INCLUDE}>)


set_target_properties(viren2d PROPERTIES
    PUBLIC_HEADER "${viren2d_PUBLIC_HEADER_FILES}"
    OUTPUT_NAME viren2d++
    DEBUG_POSTFIX "d")


if (INSTALL_viren2d)
    install(TARGETS viren2d
        EXPORT viren2dTargets
        # We don't have any installable executables, so we don't need RUNTIME
        # RUNTIME DESTINATION ${INSTALL_DIR_BIN}
        LIBRARY DESTINATION ${INSTALL_DIR_LIB}
        ARCHIVE DESTINATION ${INSTALL_DIR_LIB}
        PUBLIC_HEADER DESTINATION ${INSTALL_DIR_INCLUDE}/viren2d)
        # Since we already defined the PUBLIC include directories, the INCLUDES DESTINATION
        # is not needed (would simply cause a duplicated path entry in the generated
        # .cmake file
        # INCLUDES DESTINATION ${INSTALL_INCLUDE_DIR}

    install(EXPORT viren2dTargets
        FILE viren2d++Targets.cmake
        NAMESPACE viren2d::
        DESTINATION ${INSTALL_DIR_CMAKE})
endif()


### Set up the CMake config file, so that viren2d++ can easily be integrated in your projects
include(CMakePackageConfigHelpers)
# Generate the version file for the config file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/viren2d++ConfigVersion.cmake"
    VERSION "${viren2d_VERSION}"
    COMPATIBILITY AnyNewerVersion)

# Create config file
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/viren2d++Config.cmake"
    INSTALL_DESTINATION ${INSTALL_DIR_CMAKE})

# Install config files
if (INSTALL_viren2d)
    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/viren2d++Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/viren2d++ConfigVersion.cmake"
        DESTINATION ${INSTALL_DIR_CMAKE})

    # Generate the export targets for the build tree
    export(EXPORT viren2dTargets
        FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/viren2d++Targets.cmake"
        NAMESPACE viren2d::)
endif()



#########################################################################################
# C++ demo

if (viren2d_WITH_DEMO)
    add_executable(viren2d_demo
        examples/demo2d.cpp)

    target_link_libraries(viren2d_demo
        PRIVATE viren2d::viren2d)
endif()



#########################################################################################
# Python bindings

if (viren2d_WITH_PYTHON)
    # pybind11 provides add_module which is basically a wrapper of CMake's add_library
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/pybind11)

    pybind11_add_module(viren2d_python
        src/bindings.cpp)

    target_compile_definitions(viren2d_python PRIVATE
        viren2d_VERSION_INFO=${viren2d_VERSION}
        viren2d_PYMODULE_NAME=viren2d)

    set_target_properties(viren2d_python PROPERTIES
        OUTPUT_NAME viren2d)

    target_link_libraries(viren2d_python PRIVATE viren2d::viren2d)
endif()


#########################################################################################
# Tests for the C++ library

if (viren2d_WITH_TESTS)
    # Prevent gtest from adding itself to the install target
    set(INSTALL_GTEST OFF)
    set(BUILD_GMOCK OFF)
    
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG main)
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    
    enable_testing()

    add_executable(viren2d_test
      tests/color_test.cpp)

    target_link_libraries(viren2d_test
      PRIVATE
      gtest_main
      viren2d::viren2d)

    include(GoogleTest)
    gtest_discover_tests(viren2d_test)
endif()


#########################################################################################
# Add uninstall target
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()



#FIXME FIXME
#FIXME FIXME
#FIXME FIXME double-check previous cmake "leftovers":
#FIXME FIXME
#FIXME FIXME
#FIXME FIXME

# ./cmake includes custom Find<...> modules, e.g. for Cairo
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Include the library headers for all targets
#include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

# We use the header-only stb library for image data handling
#include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libs/stb")


#TODO add cairo to config.cmake!!
# ./cmake includes a custom FindCairo module
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
#find_package(Cairo REQUIRED)
#include_directories(${Cairo_INCLUDE_DIRS})

#TODO maybe needed when we start working on camera calibration, 3d stuff
#find_package(Eigen3 REQUIRED)
#include_directories(${EIGEN3_INCLUDE_DIR})


#########################################################################################
# C++ demo/examples

#TODO make library, then include library in demo
#TODO post config/build step - copy image/other assets to build folder
#add_executable(demo2d
#    ${viren2d_PUBLIC_HEADER_FILES}
#    ${viren2d_PRIVATE_HEADER_FILES}
#    ${viren2d_SOURCE_FILES}
#    examples/demo2d.cpp
#)
# TODO When we have a standalone cpp library, it expose/link/... Cairo_LIBS. Then, the following is no longer needed!
#target_link_libraries(demo2d PRIVATE ${Cairo_LIBS})

#TODO check interface/public/private https://stackoverflow.com/a/49538748/400948
#TODO read https://kubasejdak.com/modern-cmake-is-like-inheritance
# OpenCV is optional, we only need it for displaying the rendered images
#find_package(OpenCV)
#if(${OpenCV_FOUND})
#    add_definitions(-DWITH_OPENCV)
#    target_include_directories(demo2d PRIVATE ${OpenCV_INCLUDE_DIRS})
#    target_link_libraries(demo2d PRIVATE ${OpenCV_LIBRARIES})
#endif()


#########################################################################################
# Python bindings

# pybind11 provides add_module which creates a target
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/pybind11)
#pybind11_add_module(viren2d    ${viren2d_PUBLIC_HEADER_FILES}    ${viren2d_PRIVATE_HEADER_FILES}    ${viren2d_SOURCE_FILES}    src/viren2d/bindings.cpp)

#target_link_libraries(viren2d PRIVATE ${Cairo_LIBS})

#target_compile_definitions(viren2d
#                           PRIVATE VERSION_INFO=${PYVIREN2D_VERSION_INFO})

