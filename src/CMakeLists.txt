cmake_minimum_required(VERSION 3.19)

file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${Herald_SOURCE_DIR}/include/Herald/*.hpp")

add_library(herald_library "Herald.cpp" "LogToStdErr.cpp" ${HEADER_LIST})

target_include_directories(herald_library PUBLIC ../include)

# target_link_libraries(herald_library PRIVATE ...) # When RapidJSON or http clients are needed

target_compile_features(herald_library PUBLIC cxx_std_17)

source_group(
    TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Header Files"
    FILES ${HEADER_LIST}
)
