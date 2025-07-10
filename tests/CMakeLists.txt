# Find Python3 interpreter (required)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

# Define path to generated tests CMake file
set(GENERATED_TESTS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/generated_tests.cmake")

# Create the generated tests file if it does not exist (empty file)
if(NOT EXISTS ${GENERATED_TESTS_FILE})
    file(WRITE ${GENERATED_TESTS_FILE} "")
endif()

# Define the path to the directory containing input data files
# These variables can be set externally when configuring CMake
set (DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data" CACHE PATH "Path to data directory")

# Define the path to the directory where test results will be stored
# These variables can be set externally when configuring CMake
set (TEST_RESULTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test_results" CACHE PATH "Path to test_results directory")

# Define the path to the directory containing tables for test results
# These variables can be set externally when configuring CMake
set (TABLES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tables" CACHE PATH "Path to tables directory")


# Include the generated tests file
include(${GENERATED_TESTS_FILE})

# Custom target to merge test results into a CSV file
add_custom_target(merge-results
  COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/merge_results.py
    --test-results-dir ${CMAKE_CURRENT_SOURCE_DIR}/test_results
    --tables-dir ${CMAKE_CURRENT_SOURCE_DIR}/tables
    --output-name output
    --reset
  COMMENT "Merge test results."
)

# Custom target to generate CMake tests from CSV input
add_custom_target(generate-tests
  COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/generate_tests.py
    --input-csv-file ${CMAKE_CURRENT_SOURCE_DIR}/tests_parameters.csv
    --output-cmake-file ${CMAKE_CURRENT_SOURCE_DIR}/generated_tests.cmake
    --data-dir ${DATA_DIR}
    --test-results-dir ${TEST_RESULTS_DIR}
    --tmp-dir ${CMAKE_CURRENT_BINARY_DIR}/tmp

  COMMENT "Generate tests."
)

# Custom target to generate metadata JSON files
add_custom_target(generate-metadata
  COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/generate_metadata.py
    --data-path ${CMAKE_CURRENT_SOURCE_DIR}/data
    --reset

  COMMENT "Generate tests."
)

# Custom target to clean previous test results
# This command is not portable on Windows
add_custom_target(clean-results
  COMMAND rm -rf ${CMAKE_CURRENT_SOURCE_DIR}/test_results/*
  COMMENT "Clean results."
)