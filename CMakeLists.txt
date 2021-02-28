cmake_minimum_required(VERSION 3.19)

project(herald LANGUAGES CXX)

#setup testing
enable_testing()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

add_subdirectory(src)

add_subdirectory(tests)
