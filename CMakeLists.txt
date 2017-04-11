cmake_minimum_required(VERSION 3.5.0)

project(gram-math)

set(CMAKE_CXX_STANDARD 14)

include(ExternalProject)
find_package(Git REQUIRED)

ExternalProject_Add(gram
        PREFIX ${gram-math_SOURCE_DIR}/lib/gram
        GIT_REPOSITORY https://github.com/jansvoboda11/gram
        GIT_TAG master
        UPDATE_COMMAND ""
        INSTALL_COMMAND ""
        BUILD_ALWAYS 1
        )

ExternalProject_Add(tinyexpr
        PREFIX ${gram-math_SOURCE_DIR}/lib/tinyexpr
        GIT_REPOSITORY https://github.com/codeplea/tinyexpr
        GIT_TAG master
        CONFIGURE_COMMAND ""
        UPDATE_COMMAND ""
        INSTALL_COMMAND ""
        BUILD_COMMAND ""
        )

add_subdirectory(src)
