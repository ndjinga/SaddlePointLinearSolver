find_package(Python3 REQUIRED COMPONENTS Interpreter)

add_custom_target(merge_results
  COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/merge_results.py
    --results-dir ${CMAKE_CURRENT_SOURCE_DIR}/results
    --tables-dir ${CMAKE_CURRENT_SOURCE_DIR}/tables
    --output-name output
    --reset
  COMMENT "Merge test results."
)


#enable_testing()

#set(TEST_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data)
#set(TEST_RESULT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/results)
#set(TEST_TMP_PATH ${CMAKE_BINARY_DIR}/tests/tmp/output.json)

#set(TEST_ID test1)
#add_test(
#  NAME ${TEST_ID}
#  COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/run_one_test.py
#    --test-id ${TEST_ID}
#    --executable ${CMAKE_BINARY_DIR}/SaddlePointLinearSolver_test
#    --matrix-name Assembly_PolyMAC_1_vp
#    --matrix-type AssemblyMesh
#    --n-proc 1
#    --data ${TEST_DATA_DIR}
#    --result-path ${TEST_RESULT_DIR}
#    --tmp-path ${TEST_TMP_PATH}
#)


#python3 ../SaddlePointLinearSolver_SRC/scripts/run_one_test.py \
#    --test-id test \
#    --executable /home/marwane/Desktop/CEA/projet/build/SaddlePointLinearSolver_test \
#    --matrix-name Assembly_PolyMAC_1_vp \
#    --matrix-type AssemblyMesh \
#    --n-proc 1 \
#    --data ../SaddlePointLinearSolver_SRC/tests/data/ \
#    --result-path ../SaddlePointLinearSolver_SRC/tests/results \
#    --tmp-path tmp/output.json

include(${CMAKE_CURRENT_SOURCE_DIR}/generated_tests.cmake)