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

if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|amd64|AMD64")
    set(ARCHITECTURE_NAME "x64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86|i386|i686")
    set(ARCHITECTURE_NAME "x86")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "armv7")
    set(ARCHITECTURE_NAME "arm")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
    set(ARCHITECTURE_NAME "arm64")
else()
    set(ARCHITECTURE_NAME "${CMAKE_SYSTEM_PROCESSOR}")
endif()

# Map CMAKE_SYSTEM_NAME to standard platform names
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(PLATFORM_NAME "Windows")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(PLATFORM_NAME "Linux")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(PLATFORM_NAME "macOS")
else()
    set(PLATFORM_NAME "${CMAKE_SYSTEM_NAME}")
endif()

set(PACKAGE_TOPLEVEL_DIR ${CMAKE_BINARY_DIR}/package)
set(PACKAGE_DIR ${PACKAGE_TOPLEVEL_DIR})
set(INCLUDE_DIR ${PACKAGE_DIR}/include)

add_custom_target(clean-package
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${PACKAGE_TOPLEVEL_DIR}
)

add_custom_target(package ALL

    # Create include directories
    COMMAND ${CMAKE_COMMAND} -E make_directory ${INCLUDE_DIR}/Herald
    COMMAND ${CMAKE_COMMAND} -E make_directory ${INCLUDE_DIR}/rapidjsoncpp
    COMMAND ${CMAKE_COMMAND} -E make_directory ${INCLUDE_DIR}/rapidjson

    # Copy headers
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/include/Herald ${INCLUDE_DIR}/Herald
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${rapidjsoncpp_SOURCE_DIR}/include/rapidjsoncpp ${INCLUDE_DIR}/rapidjsoncpp
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${RapidJSON_SOURCE_DIR}/include/rapidjson ${INCLUDE_DIR}/rapidjson

    # Create library directory using generator expressions
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PACKAGE_DIR}/lib/${PLATFORM_NAME}/${ARCHITECTURE_NAME}/$<CONFIG>

    # Copy the built library
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:herald_library> ${PACKAGE_DIR}/lib/${PLATFORM_NAME}/${ARCHITECTURE_NAME}/$<CONFIG>

    # Invoke the packaging script with CONFIGURATION as a variable
    COMMAND ${CMAKE_COMMAND} -D PACKAGE_DIR=${PACKAGE_DIR} -D PLATFORM_NAME=${PLATFORM_NAME} -D ARCHITECTURE_NAME=${ARCHITECTURE_NAME} -D CONFIGURATION=$<CONFIG> -P ${CMAKE_SOURCE_DIR}/package.cmake

    # Optional: Print a message for debugging
    COMMAND ${CMAKE_COMMAND} -E echo "Packaging for configuration: $<CONFIG>"
)

# Ensure the package target depends on the library
add_dependencies(package herald_library)

add_subdirectory(src)

add_subdirectory(tests)
