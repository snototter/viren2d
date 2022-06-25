function(setup_werkzeugkiste)
    # Checks if werkzeugkiste is installed. If not, it will be fetched from 
    # github and integrated into our build step.
    find_package(werkzeugkiste 0.5 QUIET)
    if(werkzeugkiste_FOUND)
        message(STATUS "[viren2d] Found locally installed werkzeugkiste v${werkzeugkiste_VERSION}.")
    else()
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
        message(STATUS "[viren2d] Downloaded werkzeugkiste v${werkzeugkiste_VERSION} via FetchContent.")
    endif()
endfunction()


function(setup_spdlog)
    # spdlog could already be available (e.g. via system package) or the user
    # manually downloaded and installed (either header-only or precompiled) the
    # logging library (or adjusted the CMake search path accordingly).
    # To accomodate this, we try the standard search workflow:
    # * Check if Cmake finds spdlog locally installed
    # TODO remove spdlog from third-party - instead, try finding it quietly, if it fails, set up
    # header-only via fetch_content! 
    # If you installed (via cmake --install) spdlog to the default location, CMake will
    # find spdlog. For other usage scenarios, refer to the spdlog examples, e.g.
    # https://github.com/gabime/spdlog/blob/v1.x/example/CMakeLists.txt
    
    find_package(spdlog QUIET)
    if(spdlog_FOUND)
        message(STATUS "[viren2d] Found locally installed spdlog.")
    else()
        # Configure spdlog
        set(SPDLOG_BUILD_EXAMPLE OFF)
        set(SPDLOG_BUILD_TESTS OFF)
        set(SPDLOG_INSTALL OFF)

        # Fetch the library:
        # IMPORTANT: we need the v1 branch for now (there's a major refactoring
        # going on - had issues with the initialization when trying an early v2)
        include(FetchContent)
        FetchContent_Declare(
            spdlog
            GIT_REPOSITORY https://github.com/gabime/spdlog.git
            GIT_TAG v1.x)
        FetchContent_MakeAvailable(spdlog)
        message(STATUS "[viren2d] Downloaded spdlog via FetchContent.")
    endif()

    # Set the available target to link against.
    # Start with header-only which should be available, no matter how/where
    # we found it.
    if(TARGET spdlog::spdlog_header_only)
        set(viren2d_SPDLOG_TARGET spdlog::spdlog_header_only)
    endif()
    # Then, replace the target, if the compiled spdlog version is available
    if(TARGET spdlog::spdlog)
        set(viren2d_SPDLOG_TARGET spdlog::spdlog)
    endif()
    
    # Currently, it is not possible to disable logging (i.e. exclude
    # any missing #include's) in viren2d. Thus, we have to ensure that
    # spdlog can be integrated:
    if(NOT TARGET ${viren2d_SPDLOG_TARGET})
        message(FATAL_ERROR "[viren2d] Could neither find nor fetch spdlog, please set it up manually: https://github.com/gabime/spdlog")
    else()
        if (viren2d_SPDLOG_TARGET MATCHES "spdlog.*header_only")
            message(STATUS "[viren2d] Using the header-only spdlog, target ${viren2d_SPDLOG_TARGET}.")
        else()
            message(STATUS "[viren2d] Using the precompiled spdlog, target ${viren2d_SPDLOG_TARGET}.")
        endif()
    endif()
endfunction()
