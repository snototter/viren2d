function(setup_werkzeugkiste)
    find_package(werkzeugkiste 0.5 QUIET)
    if(werkzeugkiste_FOUND)
        message(STATUS "Using werkzeugkiste v${werkzeugkiste_VERSION} (locally installed)")
    else()
        # We only need the toolbox (no examples, no testing)
        set(werkzeugkiste_WITH_EXAMPLES OFF)
        set(werkzeugkiste_WITH_TESTS OFF)
        # We need werkzeugkiste's install targets (its vector class
        # is included in the public viren2d++ interface).
        set(INSTALL_werkzeugkiste ON)

        # Fetch the library:
        include(FetchContent)
        FetchContent_Declare(
        werkzeugkiste
        GIT_REPOSITORY https://github.com/snototter/werkzeugkiste.git
        GIT_TAG main
        )
        FetchContent_MakeAvailable(werkzeugkiste)
        message(STATUS "Using werkzeugkiste v${werkzeugkiste_VERSION} (via FetchContent)")
    endif()
endfunction()



function(setup_spdlog)
    # TODO remove spdlog from third-party - instead, try finding it quietly, if it fails, set up
    # header-only via fetch_content! 
    # If you installed (via cmake --install) spdlog to the default location, CMake will
    # find spdlog. For other usage scenarios, refer to the spdlog examples, e.g.
    # https://github.com/gabime/spdlog/blob/v1.x/example/CMakeLists.txt
    find_package(spdlog REQUIRED)
    target_link_libraries(${TARGET_CPP_LIB} PRIVATE spdlog::spdlog)
endfunction()


function(setup_stb)
    message(WARNING "TODO stb should also be fetched like the other dependencies")
endfunction()