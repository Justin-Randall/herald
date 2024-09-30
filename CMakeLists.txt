# Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
# MIT License, see LICENSE file for full details.

cmake_minimum_required(VERSION 3.19)

project(herald LANGUAGES CXX)

# Ensure Source control hooks are configured
# (just git for now)
add_custom_target(LocalGitConfiguration ALL
    COMMAND ${CMAKE_COMMAND} -E echo "Configuring Git Hooks"

    # Using --local ensures this applies to this repository only
    COMMAND git config --local core.hooksPath ${CMAKE_SOURCE_DIR}/scripts/source_control/githooks
)

# setup testing
include(CTest)

enable_testing()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

set(JSONCPP_DISABLE_TESTING true)

include(FetchContent)
FetchContent_Declare(
    rapidjsoncpp
    GIT_REPOSITORY https://github.com/Justin-Randall/rapidjsoncpp.git
    GIT_TAG origin/main
)
FetchContent_MakeAvailable(rapidjsoncpp)

include_directories("${gtest_SOURCE_DIR}/include"
    "${Herald_SOURCE_DIR}/include/Herald/*.hpp"
    "${rapidjsoncpp_SOURCE_DIR}/include"
    "${RapidJSON_SOURCE_DIR}/include"
)

add_subdirectory(src)

add_subdirectory(tests)
