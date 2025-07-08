# Find Python3 interpreter (required)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

# Define path to generated tests CMake file
set(GENERATED_TESTS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/generated_tests.cmake")

# Create the generated tests file if it does not exist (empty file)
if(NOT EXISTS ${GENERATED_TESTS_FILE})
    file(WRITE ${GENERATED_TESTS_FILE} "")
endif()

# Include the generated tests file
include(${GENERATED_TESTS_FILE})

# Custom target to merge test results into a CSV file
add_custom_target(merge-results
  COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/merge_results.py
    --results-dir ${CMAKE_CURRENT_SOURCE_DIR}/results
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
    --data-dir ${CMAKE_CURRENT_SOURCE_DIR}/data
    --test-results-dir ${CMAKE_CURRENT_SOURCE_DIR}/results
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
  COMMAND rm -rf ${CMAKE_CURRENT_SOURCE_DIR}/results/*
  COMMENT "Clean results."
)