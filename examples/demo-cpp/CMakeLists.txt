message(WARNING "FIXME needs research how to check for PARENT_DIRECTORY with modern cmake; v2d target name variable: '${viren2d_TARGET_CPP_DEMO}'")
if(${viren2d_TARGET_CPP_DEMO})
  message(STATUS "[viren2d] Configuring demo application.")
else()
  message(WARNING "Standalone build of this demo is not yet supported - tasks: 1) set up project 2) fetch viren2d") #FIXME change to fatal error
  set(viren2d_TARGET_CPP_DEMO demo-viren2d)
endif()


add_executable(${viren2d_TARGET_CPP_DEMO}
    ${CMAKE_CURRENT_SOURCE_DIR}/demo_main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/demo_utils/demos.h
    ${CMAKE_CURRENT_SOURCE_DIR}/demo_utils/demos_imagebuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/demo_utils/demos_lines.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/demo_utils/demos_shapes.cpp)


target_link_libraries(${viren2d_TARGET_CPP_DEMO}
    PRIVATE ${viren2d_TARGET_CPP_LIB}::${viren2d_TARGET_CPP_LIB})


# OpenCV is optional, we only use it to display the rendered images
find_package(OpenCV QUIET)
if(${OpenCV_FOUND})
    message(STATUS "[viren2d] Demo application will use OpenCV v${OpenCV_VERSION} to display images.")

    target_compile_definitions(${viren2d_TARGET_CPP_DEMO}
        PRIVATE viren2d_WITH_OPENCV)

    target_link_libraries(${viren2d_TARGET_CPP_DEMO}
        PRIVATE ${OpenCV_LIBRARIES})
endif()
