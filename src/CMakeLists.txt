# Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
# MIT License, see LICENSE file for full details.

cmake_minimum_required(VERSION 3.19)

file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${Herald_SOURCE_DIR}/include/Herald/*.hpp")

add_library(herald_library

    # "Herald.cpp"
    "SimpleLogTransformer.cpp"
    "JsonLogTransformer.cpp"
    "BaseLogTransformer.cpp"
    "BaseLogTransformerBuilder.cpp"
    "FileLogWriter.cpp"
    ${HEADER_LIST})

target_include_directories(herald_library PUBLIC ../include)
include_directories("${gtest_SOURCE_DIR}/include"
    "${Herald_SOURCE_DIR}/include/Herald/*.hpp"
    "${rapidjsoncpp_SOURCE_DIR}/include"
    "${RapidJSON_SOURCE_DIR}/include"
)

# target_link_libraries(herald_library PRIVATE ...) # When RapidJSON or http clients are needed
target_compile_features(herald_library PUBLIC cxx_std_17)

# Set maximum warning levels and treat warnings as errors
if(MSVC)
    target_compile_options(herald_library PRIVATE /W4 /WX)
else()
    target_compile_options(herald_library PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

source_group(
    TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Header Files"
    FILES ${HEADER_LIST}
)
