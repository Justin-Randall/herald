# Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
# MIT License, see LICENSE file for full details.

# Ensure that all required variables are set
if(NOT DEFINED PACKAGE_DIR)
    message(FATAL_ERROR "PACKAGE_DIR is not defined.")
endif()

if(NOT DEFINED PLATFORM_NAME)
    message(FATAL_ERROR "PLATFORM_NAME is not defined.")
endif()

if(NOT DEFINED ARCHITECTURE_NAME)
    message(FATAL_ERROR "ARCHITECTURE_NAME is not defined.")
endif()

if(NOT DEFINED CONFIGURATION)
    message(FATAL_ERROR "CONFIGURATION is not defined.")
endif()

# Display the received variables for debugging
message(STATUS "Package Directory: ${PACKAGE_DIR}")
message(STATUS "Platform Name: ${PLATFORM_NAME}")
message(STATUS "Architecture Name: ${ARCHITECTURE_NAME}")
message(STATUS "Configuration: ${CONFIGURATION}")

# Define the library and pdb paths
set(LIBRARY_PATH "${CMAKE_BINARY_DIR}/src/${CONFIGURATION}/herald_library.lib")
set(PDB_PATH "${CMAKE_BINARY_DIR}/src/${CONFIGURATION}/herald_library.pdb")

# Create the package directories (redundant if already created in CMakeLists.txt, but safe)
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${PACKAGE_DIR}/lib/${PLATFORM_NAME}/${ARCHITECTURE_NAME}/${CONFIGURATION}")

# Copy the library if it exists
if(EXISTS "${LIBRARY_PATH}")
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different "${LIBRARY_PATH}" "${PACKAGE_DIR}/lib/${PLATFORM_NAME}/${ARCHITECTURE_NAME}/${CONFIGURATION}/")
    message(STATUS "Copied library: ${LIBRARY_PATH}")
else()
    message(WARNING "Library file not found: ${LIBRARY_PATH}")
endif()

# Copy the PDB file if it exists
if(EXISTS "${PDB_PATH}")
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different "${PDB_PATH}" "${PACKAGE_DIR}/lib/${PLATFORM_NAME}/${ARCHITECTURE_NAME}/${CONFIGURATION}/")
    message(STATUS "Copied PDB file: ${PDB_PATH}")
else()
    message(STATUS "PDB file not found (might not be generated for this configuration): ${PDB_PATH}")
endif()
