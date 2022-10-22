cmake_minimum_required(VERSION 3.15)

###############################################################################
# Project definitions
###############################################################################

# This build's version could have already been set, e.g. by the CMakeBuild
# extension in `setup.py`
if(NOT viren2d_VERSION)
  file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" viren2d_VERSION)
endif()
  
# Set up the project
project(viren2d VERSION ${viren2d_VERSION} LANGUAGES CXX)

# -----------------------------------------------------------------------------
# C++ library and CMake namespace is "viren2d++""
# Python bindings/library is "viren2d"
set(viren2d_TARGET_CPP_LIB  ${PROJECT_NAME}++)
set(viren2d_TARGET_CPP_TEST ${PROJECT_NAME}-test)
set(viren2d_TARGET_CPP_DEMO demo-${PROJECT_NAME})
set(viren2d_TARGET_PYTHON_LIB ${PROJECT_NAME})


# -----------------------------------------------------------------------------
# User options

option(viren2d_BUILD_DEMO "Build C++ demo." OFF)
option(viren2d_BUILD_PYTHON "Build Python bindings." OFF)
option(viren2d_BUILD_TESTS "Build test suite." OFF)
option(viren2d_INSTALL "Configure installation target." OFF)

#set(viren2d_LOG_LEVEL "info" CACHE STRING "Select log level")
#set_property(CACHE viren2d_LOG_LEVEL PROPERTY STRINGS disable trace debug info warn error)


# -----------------------------------------------------------------------------
# Installation paths

# Use the common Unix default installation paths (the GNUInstallDirs package
# should also provide sane paths on Windows, but I didn't check this yet):
include(GNUInstallDirs)
set(INSTALL_DIR_INCLUDE ${CMAKE_INSTALL_INCLUDEDIR})
set(INSTALL_DIR_LIB ${CMAKE_INSTALL_LIBDIR})
# A default search location for CMake config files is /usr/lib/cmake/<package-name>
set(INSTALL_DIR_CMAKE ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})


# -----------------------------------------------------------------------------
# Compiler options

set(CMAKE_CXX_STANDARD 14)  # We use std::exchange
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Only std c++14, not gnu++14
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra") # TODO(cmake-improvement) gcc vs msvc vs... currently assumes linux, check CMakeLists of werkzeugkiste or spdlog!
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")


# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()


# Compile library with -fPIC
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


###############################################################################
# C++ library
###############################################################################

# Auto-generate the version.h file
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/include/viren2d/version.h)


# -----------------------------------------------------------------------------
# Header files

set(viren2d_PUBLIC_HEADER_FILES
    include/viren2d/colors.h
    include/viren2d/colormaps.h
    include/viren2d/drawing.h
    include/viren2d/opticalflow.h
    include/viren2d/imagebuffer.h
    include/viren2d/primitives.h
    include/viren2d/positioning.h
    include/viren2d/styles.h
    include/viren2d/version.h
    include/viren2d/viren2d.h)


set(viren2d_PRIVATE_HEADER_FILES
    src/helpers/logging.h
    src/helpers/color_conversion.h
    src/helpers/colormaps_helpers.h
    src/helpers/drawing_helpers.h
    src/helpers/imagebuffer_helpers.impl.h
    src/helpers/enum.h)


# -----------------------------------------------------------------------------
# Source files
set(viren2d_SOURCE_FILES
    src/primitives.cpp
    src/colors.cpp
    src/colormaps.cpp
    src/drawing.cpp
    src/opticalflow.cpp
    src/imagebuffer.cpp
    src/positioning.cpp
    src/styles.cpp
    src/helpers/colormaps_helpers.cpp
    src/helpers/drawing_helpers_text.cpp
    src/helpers/drawing_helpers_image.cpp
    src/helpers/drawing_helpers_detection_tracking.cpp
    src/helpers/drawing_helpers_pinhole.cpp
    src/helpers/drawing_helpers_primitives.cpp)


# -----------------------------------------------------------------------------
# viren2d C++ library

add_library(${viren2d_TARGET_CPP_LIB}
    ${viren2d_PUBLIC_HEADER_FILES}
    ${viren2d_PRIVATE_HEADER_FILES}
    ${viren2d_SOURCE_FILES})

# Add a namespace'd alias
add_library(${viren2d_TARGET_CPP_LIB}::${viren2d_TARGET_CPP_LIB} ALIAS ${viren2d_TARGET_CPP_LIB})

set_target_properties(${viren2d_TARGET_CPP_LIB}
    PROPERTIES
    PUBLIC_HEADER "${viren2d_PUBLIC_HEADER_FILES}"
    DEBUG_POSTFIX "d")


###############################################################################
# Dependencies
###############################################################################

# The ./cmake subfolder contains a custom FindCairo module
# TODO(windows-support) the current FindCairo version will *NOT* work
#   out-of-the-box on Windows. Fixing this is a low-priority task, as it
#   will likely be solved when distributing via conda
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(Cairo REQUIRED)
target_link_libraries(${viren2d_TARGET_CPP_LIB} PRIVATE Cairo::Cairo)

# -----------------------------------------------------------------------------
# Set up the remaining external targets to include
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/setup_dependencies.cmake)
setup_werkzeugkiste(${viren2d_TARGET_CPP_LIB})
setup_spdlog(${viren2d_TARGET_CPP_LIB})

# -----------------------------------------------------------------------------
# Set up include paths

# Note (summary CMake documentation):
# * PRIVATE is for viren2d-internal usage only
# * PUBLIC BUILD_INTERFACE is for scenarios, where another project
#   includes this CMakeLists.txt
# * PUBLIC INSTALL_INTERFACE is for typical library usage, i.e.
#   viren2d has previously been installed and should now just be linked
#   into the user's application
target_include_directories(${viren2d_TARGET_CPP_LIB}
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/libs/stb"
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${INSTALL_DIR_INCLUDE}>)

# Note that STB (https://github.com/nothings/stb) is a pure header-only library.
# We don't need to set it up (and can't easily fetch it via CMake).
# Thus, it is included as a submodule (license-wise, we could also distribute
# the header file(s) along with viren2d).
# To set it up, we only need to adjust the PRIVATE include path as above.


###############################################################################
# C++ library installation
###############################################################################

if(viren2d_INSTALL)
    install(
        TARGETS ${viren2d_TARGET_CPP_LIB}
        EXPORT ${viren2d_TARGET_CPP_LIB}Targets
        
        # We don't have any installable executables, so we don't need RUNTIME
        # RUNTIME DESTINATION ${INSTALL_DIR_BIN}
        LIBRARY DESTINATION ${INSTALL_DIR_LIB}
        ARCHIVE DESTINATION ${INSTALL_DIR_LIB}
        PUBLIC_HEADER DESTINATION ${INSTALL_DIR_INCLUDE}/viren2d)

        # Since we already defined the PUBLIC include directories,
        # the INCLUDES DESTINATION is not needed. It would simply cause a
        # duplicated path entry in the generated .cmake file
        # # INCLUDES DESTINATION ${INSTALL_INCLUDE_DIR}

    install(
        EXPORT ${viren2d_TARGET_CPP_LIB}Targets
        FILE ${viren2d_TARGET_CPP_LIB}Targets.cmake
        NAMESPACE ${viren2d_TARGET_CPP_LIB}::
        DESTINATION ${INSTALL_DIR_CMAKE})

    # Add "uninstall" target
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_utils.cmake)
    add_generic_uninstall_target()
endif()

# -----------------------------------------------------------------------------
# CMake package configuration

# Set up the CMake config file, so that viren2d++ can be
# swiftly integrated in your projects
include(CMakePackageConfigHelpers)

# Generate the version file for the config file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${viren2d_TARGET_CPP_LIB}ConfigVersion.cmake"
    VERSION "${viren2d_VERSION}"
    COMPATIBILITY AnyNewerVersion)

# Create config file
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/${viren2d_TARGET_CPP_LIB}Config.cmake"
    INSTALL_DESTINATION ${INSTALL_DIR_CMAKE})

# Install config files
if(viren2d_INSTALL)
    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${viren2d_TARGET_CPP_LIB}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${viren2d_TARGET_CPP_LIB}ConfigVersion.cmake"
        DESTINATION ${INSTALL_DIR_CMAKE})

    # Generate the export targets for the build tree
    export(EXPORT ${viren2d_TARGET_CPP_LIB}Targets
        FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/${viren2d_TARGET_CPP_LIB}Targets.cmake"
        NAMESPACE ${viren2d_TARGET_CPP_LIB}::)
endif()


###############################################################################
# C++ demo
###############################################################################

if(viren2d_BUILD_DEMO)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples/demo-cpp)
endif()


###############################################################################
# Python bindings
###############################################################################

if(viren2d_BUILD_PYTHON)
    # pybind11 provides add_module which is basically a wrapper of CMake's add_library
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/pybind11)

    pybind11_add_module(${viren2d_TARGET_PYTHON_LIB}
        src/bindings/binding_helpers.h
        src/bindings/bindings_colors.cpp
        src/bindings/bindings_colormaps.cpp
        src/bindings/bindings_imagebuffer.cpp
        src/bindings/bindings_opticalflow.cpp
        src/bindings/bindings_primitives.cpp
        src/bindings/bindings_styles.cpp
        src/bindings/bindings_text.cpp
        src/bindings/bindings_painter.cpp
        src/bindings/bindings_vector.cpp
        src/bindings.cpp)
        
    setup_spdlog(${viren2d_TARGET_PYTHON_LIB})

    target_compile_definitions(${viren2d_TARGET_PYTHON_LIB} PRIVATE
        viren2d_VERSION_INFO=${viren2d_VERSION}
        viren2d_PYMODULE_NAME=${PROJECT_NAME})

    target_include_directories(${viren2d_TARGET_PYTHON_LIB} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src")

    set_target_properties(${viren2d_TARGET_PYTHON_LIB} PROPERTIES
        OUTPUT_NAME ${PROJECT_NAME})

    target_link_libraries(${viren2d_TARGET_PYTHON_LIB}
        PRIVATE ${viren2d_TARGET_CPP_LIB}::${viren2d_TARGET_CPP_LIB})
endif()


###############################################################################
# Tests for the C++ library
###############################################################################

if(viren2d_BUILD_TESTS)
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

    add_executable(${viren2d_TARGET_CPP_TEST}
        src/helpers/enum.h
        tests/color_test.cpp
        tests/colormaps_test.cpp
        tests/primitives_test.cpp
        tests/imagebuffer_test.cpp
        tests/utils_test.cpp
        tests/style_test.cpp)

    target_include_directories(${viren2d_TARGET_CPP_TEST}
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

    target_link_libraries(${viren2d_TARGET_CPP_TEST}
        PRIVATE
        gtest_main
        werkzeugkiste::strings
        werkzeugkiste::container
        ${viren2d_TARGET_CPP_LIB}::${viren2d_TARGET_CPP_LIB})

    target_compile_definitions(${viren2d_TARGET_CPP_TEST} PRIVATE
        viren2d_EXAMPLE_IMAGE_FILE="${CMAKE_CURRENT_SOURCE_DIR}/examples/data/flamingo.jpg"
        viren2d_EXAMPLE_IMAGE_WIDTH=400
        viren2d_EXAMPLE_IMAGE_HEIGHT=400)

    include(GoogleTest)
    gtest_discover_tests(${viren2d_TARGET_CPP_TEST})
endif()
