

if(NOT DEFINED HERALD_DISABLE_TESTING)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  include(FetchContent)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        e2239ee6043f73722e7aa812a459f54a28552929 # release-1.10.0
  )
  FetchContent_MakeAvailable(googletest)

  add_executable(herald_tests "test_herald.cpp" "test_json.cpp")

  target_compile_features(herald_tests PRIVATE cxx_std_20)

  target_link_libraries(herald_tests PRIVATE gtest_main herald_library rapidjsoncpp)

  set(UNIT_TEST herald_tests)
  add_test(NAME ${UNIT_TEST} COMMAND ${UNIT_TEST})

  include_directories("${gtest_SOURCE_DIR}/include"
      "${Herald_SOURCE_DIR}/include/Herald/*.hpp"
      "${rapidjsoncpp_SOURCE_DIR}/include"
      "${rapidjson_SOURCE_DIR}/include"
  )


  find_program(OpenCppCoverage_BINARY OpenCppCoverage.exe)

  set(TEST_COMMAND_LINE_OPTIONS ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION> -R "^${UNIT_TEST}$" --output-on-failures)

  if(OpenCppCoverage_BINARY)
    add_custom_command(
      TARGET ${UNIT_TEST}
      COMMENT "Run coverage report and tests"
      POST_BUILD 
      WORKING_DIRECTORY ${CMAKE_TEST_DIRECTORY}
      COMMAND ${OpenCppCoverage_BINARY} --quiet --export_type cobertura:${CMAKE_SOURCE_DIR}/build/coverage.xml --cover_children --sources herald --excluded_modules system32 --excluded_sources build\\_deps --excluded_sources googletest\\include\\gtest -- ${TEST_COMMAND_LINE_OPTIONS}
      )
  else()
    add_custom_command(
      TARGET ${UNIT_TEST}
      COMMENT "Run tests"
      POST_BUILD 
      WORKING_DIRECTORY ${CMAKE_TEST_DIRECTORY}
      COMMAND ${TEST_COMMAND_LINE_OPTIONS}
    )
  endif()
endif()
