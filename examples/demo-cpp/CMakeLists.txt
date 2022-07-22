cmake_minimum_required(VERSION 3.15)

if(TARGET ${viren2d_TARGET_CPP_LIB})
  message(STATUS "[viren2d] Configuring demo application.")
else()
  message(STATUS "[viren2d] Standalone demo application, fetching viren2d via github.")

  set(viren2d_TARGET_CPP_DEMO demo-viren2d)
   
  # Let CMake download and set up the viren2d dependency
  include(FetchContent)
  FetchContent_Declare(
    viren2d
    GIT_REPOSITORY https://github.com/snototter/viren2d.git
    GIT_TAG main)
  FetchContent_MakeAvailable(viren2d)
endif()


add_executable(${viren2d_TARGET_CPP_DEMO}
    ${CMAKE_CURRENT_SOURCE_DIR}/demo_main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/demo_utils/demos.h
    ${CMAKE_CURRENT_SOURCE_DIR}/demo_utils/demos_color.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/demo_utils/demos_colorization.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/demo_utils/demos_imagebuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/demo_utils/demos_lines.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/demo_utils/demos_pinhole.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/demo_utils/demos_shapes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/demo_utils/demos_text.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/demo_utils/demos_tracking_by_detection.cpp)

target_compile_definitions(${viren2d_TARGET_CPP_DEMO}
    PRIVATE
    viren2d_EXAMPLE_DATA_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../data/")


target_include_directories(${viren2d_TARGET_CPP_DEMO}
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")


target_link_libraries(${viren2d_TARGET_CPP_DEMO}
    PRIVATE
      viren2d++::viren2d++
      werkzeugkiste::files)


# OpenCV is optional, we only use it to display the rendered images
find_package(OpenCV QUIET)
if(${OpenCV_FOUND})
    message(STATUS "[viren2d] Demo application will use OpenCV v${OpenCV_VERSION} to display images.")

    target_compile_definitions(${viren2d_TARGET_CPP_DEMO}
        PRIVATE viren2d_WITH_OPENCV)

    target_link_libraries(${viren2d_TARGET_CPP_DEMO}
        PRIVATE ${OpenCV_LIBRARIES})
endif()

