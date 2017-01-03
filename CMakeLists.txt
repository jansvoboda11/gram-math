cmake_minimum_required(VERSION 3.6.0)

project(gram-math)

set(CMAKE_CXX_STANDARD 14)

include(ExternalProject)
find_package(Git REQUIRED)

ExternalProject_Add(gram
        PREFIX ${gram-math_SOURCE_DIR}/lib/gram
        GIT_REPOSITORY https://github.com/jansvoboda11/gram
        GIT_TAG master
        UPDATE_COMMAND ${GIT_EXECUTABLE} pull
        INSTALL_COMMAND ""
        )

link_directories(${gram-math_SOURCE_DIR}/lib/gram/src/gram-build/src)

add_subdirectory(src)
