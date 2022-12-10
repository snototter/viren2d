function(setup_werkzeugkiste viren2d_TARGET_CPP_LIB)
    # Checks if werkzeugkiste is installed. If not, it will be fetched from 
    # github and integrated into our build step.
    find_package(werkzeugkiste 0.5 QUIET)
    if(werkzeugkiste_FOUND)
        message(STATUS "[viren2d] Using locally installed werkzeugkiste v${werkzeugkiste_VERSION}.")
    else()
        message(STATUS "[viren2d] Including werkzeugkiste via FetchContent - initial download may take some time.")
        # We only need the toolbox (no examples, no testing)
        set(werkzeugkiste_BUILD_EXAMPLES OFF)
        set(werkzeugkiste_BUILD_TESTS OFF)
        # We need werkzeugkiste's install targets (since its vector class
        # is part of the public viren2d++ interface).
        set(werkzeugkiste_INSTALL ON)

        # Fetch the library:
        include(FetchContent)
        FetchContent_Declare(
            werkzeugkiste
            GIT_REPOSITORY https://github.com/snototter/werkzeugkiste.git
            GIT_TAG main)
        FetchContent_MakeAvailable(werkzeugkiste)
        message(STATUS "[viren2d] Using werkzeugkiste v${werkzeugkiste_VERSION} via FetchContent.")
    endif()

    # Modern CMake, we simply need to link against the targets.
    # This takes care of adjusting the include directories AND the
    # linker paths as needed.
    target_link_libraries(${viren2d_TARGET_CPP_LIB}
        PRIVATE
            werkzeugkiste::strings
            werkzeugkiste::timing
        PUBLIC
            werkzeugkiste::geometry)
endfunction()


function(setup_spdlog viren2d_TARGET_CPP_LIB)
    # spdlog could already be available (e.g. via system package) or the user
    # manually downloaded and installed (either header-only or precompiled) the
    # logging library (or adjusted the CMake search path accordingly).
    # To accomodate for these options, we use this search workflow:
    # 1) Check if CMake finds spdlog (already installed)
    # 2) If not found, fetch it from github
       
    find_package(spdlog QUIET)
    ##if(spdlog_FOUND)
    ##    message(STATUS "[viren2d] Found locally installed spdlog.")
    ##else()
    ##TODO check, then clean up the previous fetchcontent version
    ##message(STATUS "[viren2d] Including spdlog via FetchContent - initial download may take some time.")
    ### Configure spdlog
    ##set(SPDLOG_BUILD_EXAMPLE OFF)
    ##set(SPDLOG_BUILD_TESTS OFF)
    ##set(SPDLOG_INSTALL OFF)
    ##
    ### Fetch the library:
    ### IMPORTANT: we need the v1 branch for now (there's a major refactoring
    ### going on - had issues with the initialization when trying an early v2)
    ##include(FetchContent)
    ##FetchContent_Declare(
    ##    spdlog
    ##    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    ##    GIT_TAG v1.x)
    ##FetchContent_MakeAvailable(spdlog)
    #message(STATUS "[viren2d] Including spdlog from git submodule.")
    ## Configure spdlog
    #set(SPDLOG_BUILD_EXAMPLE OFF)
    #set(SPDLOG_BUILD_TESTS OFF)
    #set(SPDLOG_INSTALL ${viren2d_INSTALL})# FORCE)
    #message(WARNING "TODO check why spdlog is missing from export targets (even if forced... didn't find the proper setup yet)")
    #add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/spdlog)
    ##endif()
    
    if(spdlog_FOUND)
        message(STATUS "[viren2d] Found spdlog.")
        if(TARGET spdlog::spdlog_header_only)
            set(viren2d_SPDLOG_TARGET spdlog::spdlog_header_only)
        endif()
        # Then, replace the target, if the compiled spdlog version is available
        if(TARGET spdlog::spdlog)
            set(viren2d_SPDLOG_TARGET spdlog::spdlog)
        endif()
        
        if(TARGET ${viren2d_SPDLOG_TARGET})
            if (viren2d_SPDLOG_TARGET MATCHES "spdlog.*header_only")
                message(STATUS "[viren2d] Using the header-only spdlog, target ${viren2d_SPDLOG_TARGET}.")
            else()
                message(STATUS "[viren2d] Using the precompiled spdlog, target ${viren2d_SPDLOG_TARGET}.")
            endif()

            target_link_libraries(${viren2d_TARGET_CPP_LIB}
                PRIVATE ${viren2d_SPDLOG_TARGET})
            target_compile_definitions(${viren2d_TARGET_CPP_LIB}
                PRIVATE viren2d_ENABLE_LOGGING)
        endif()
    else()
        message(STATUS "[viren2d] spdlog not found - logging is disabled. For install instructions, see https://github.com/gabime/spdlog")
    endif()


    # Set the available target to link against.
    # Start with header-only which should be available, no matter how/where
    # we found it.
#    if(TARGET spdlog::spdlog_header_only)
#        set(viren2d_SPDLOG_TARGET spdlog::spdlog_header_only)
#    endif()
#    # Then, replace the target, if the compiled spdlog version is available
#    if(TARGET spdlog::spdlog)
#        set(viren2d_SPDLOG_TARGET spdlog::spdlog)
#    endif()
    
#    # Currently, it is not possible to disable logging (i.e. exclude
#    # any missing #include's) in viren2d. Thus, we have to ensure that
#    # spdlog can be integrated:
#    if(NOT TARGET ${viren2d_SPDLOG_TARGET})
#        message(FATAL_ERROR "[viren2d] Could neither find nor fetch spdlog, please set it up manually: https://github.com/gabime/spdlog")
#    else()
#        if (viren2d_SPDLOG_TARGET MATCHES "spdlog.*header_only")
#            message(STATUS "[viren2d] Using the header-only spdlog, target ${viren2d_SPDLOG_TARGET}.")
#        else()
#            message(STATUS "[viren2d] Using the precompiled spdlog, target ${viren2d_SPDLOG_TARGET}.")
#        endif()
#
#        target_link_libraries(${viren2d_TARGET_CPP_LIB}
#            PRIVATE ${viren2d_SPDLOG_TARGET})
#    endif()
endfunction()
