cmake_minimum_required(VERSION 3.11)

file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${Herald_SOURCE_DIR}/include/Herald/*.hpp")

add_library(${PROJECT_NAME} "Herald.cpp" "LogToStdErr.cpp" ${HEADER_LIST})

target_include_directories(${PROJECT_NAME} PUBLIC ../include)

# target_link_libraries(herald_library PRIVATE ...) # When RapidJSON or http clients are needed

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

source_group(
    TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Header Files"
    FILES ${HEADER_LIST}
)
