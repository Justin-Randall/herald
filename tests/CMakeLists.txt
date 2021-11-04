include(FetchContent)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        e2239ee6043f73722e7aa812a459f54a28552929 # release-1.10.0
)
FetchContent_MakeAvailable(googletest)

add_executable(herald_tests "test_herald.cpp" "test_json.cpp")

target_compile_features(herald_tests PRIVATE cxx_std_17)

target_link_libraries(herald_tests PRIVATE gtest_main herald_library)

set(UNIT_TEST herald_tests)
add_test(NAME ${UNIT_TEST} COMMAND ${UNIT_TEST})

include_directories("${gtest_SOURCE_DIR}/include"
    "${Herald_SOURCE_DIR}/include/Herald/*.hpp"
)

add_custom_command(
  TARGET ${UNIT_TEST}
  COMMENT "Run tests"
  POST_BUILD 
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION> -R "^${UNIT_TEST}$" --output-on-failures
)

find_program(OpenCppCoverage_BINARY OpenCppCoverage.exe)

if(OpenCppCoverage_BINARY)
 add_custom_command(
   TARGET ${UNIT_TEST}
   COMMENT "Run coverage report"
   POST_BUILD 
   WORKING_DIRECTORY ${CMAKE_TEST_DIRECTORY}
   COMMAND OpenCppCoverage.exe --quiet --export_type cobertura:${CMAKE_SOURCE_DIR}/build/coverage.xml --cover_children --sources herald --excluded_modules system32 --excluded_sources build\\_deps --excluded_sources googletest\\include\\gtest -- ctest -j10 -C Debug -T test --output-on-failure
 )
endif()
