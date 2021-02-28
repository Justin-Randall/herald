configure_file(CMakeLists.txt.in googletest-download/CmakeLists.txt)

set(GTEST_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/googletest-build")

execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

add_executable(herald_tests "test_herald.cpp")

target_compile_features(herald_tests PRIVATE cxx_std_17)

target_link_libraries(herald_tests PRIVATE gtest_main herald_library)

add_test(NAME test_herald COMMAND herald_tests)

include_directories("${gtest_SOURCE_DIR}/include"
    "${Herald_SOURCE_DIR}/include/Herald/*.hpp"
)
