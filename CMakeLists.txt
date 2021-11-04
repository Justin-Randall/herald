cmake_minimum_required(VERSION 3.19)

project(herald LANGUAGES CXX)

#setup testing
include (CTest)

enable_testing()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

set(JSONCPP_DISABLE_TESTING true)

include(FetchContent)
FetchContent_Declare(
    rapidjsoncpp
    GIT_REPOSITORY git@github.com:Justin-Randall/rapidjsoncpp.git
    GIT_TAG        156489c7803b402f4f32fb14790123167acc2fa1 # release-1.10.0
)
FetchContent_MakeAvailable(rapidjsoncpp)

include_directories("${gtest_SOURCE_DIR}/include"
    "${Herald_SOURCE_DIR}/include/Herald/*.hpp"
    "${rapidjsoncpp_SOURCE_DIR}/include"
)

add_subdirectory(src)

add_subdirectory(tests)
