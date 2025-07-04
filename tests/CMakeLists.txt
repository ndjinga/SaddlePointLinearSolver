enable_testing()

set(TEST_DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/data)
set(TEST_RESULT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/results)
set(TEST_TMP_PATH ${CMAKE_BINARY_DIR}/tests/tmp/output.json)

set(TEST_NAME test1)
add_test(
  NAME ${TEST_NAME}
  COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/run_one_test.py
    --test-id ${TEST_NAME}
    --executable ${CMAKE_BINARY_DIR}/SaddlePointLinearSolver_test
    --matrix-name Assembly_PolyMAC_1_vp
    --matrix-type AssemblyMesh
    --n-proc 1
    --data ${TEST_DATA_DIR}
    --result-path ${TEST_RESULT_DIR}
    --tmp-path ${TEST_TMP_PATH}
)

# Should fail (wrong type)
set(TEST_NAME test2)
add_test(
  NAME ${TEST_NAME}
  COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/run_one_test.py
    --test-id ${TEST_NAME}
    --executable ${CMAKE_BINARY_DIR}/SaddlePointLinearSolver_test
    --matrix-name Assembly_PolyMAC_42_vp
    --matrix-type Hexa
    --n-proc 1
    --data ${TEST_DATA_DIR}
    --result-path ${TEST_RESULT_DIR}
    --tmp-path ${TEST_TMP_PATH}
)

set(TEST_NAME test3)
add_test(
  NAME ${TEST_NAME}
  COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/run_one_test.py
    --test-id ${TEST_NAME}
    --executable ${CMAKE_BINARY_DIR}/SaddlePointLinearSolver_test
    --matrix-name Assembly_PolyMAC_2_vp
    --matrix-type AssemblyMesh
    --n-proc 1
    --data ${TEST_DATA_DIR}
    --result-path ${TEST_RESULT_DIR}
    --tmp-path ${TEST_TMP_PATH}
)

# Should fail (no metadata)
set(TEST_NAME test4)
add_test(
  NAME ${TEST_NAME}
  COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/run_one_test.py
    --test-id ${TEST_NAME}
    --executable ${CMAKE_BINARY_DIR}/SaddlePointLinearSolver_test
    --matrix-name Hexa_PolyMAC_1_vp
    --matrix-type Hexa
    --n-proc 1
    --data ${TEST_DATA_DIR}
    --result-path ${TEST_RESULT_DIR}
    --tmp-path ${TEST_TMP_PATH}
)


set(TEST_NAME test5)
add_test(
  NAME ${TEST_NAME}
  COMMAND python3 ${CMAKE_SOURCE_DIR}/scripts/run_one_test.py
    --test-id ${TEST_NAME}
    --executable ${CMAKE_BINARY_DIR}/SaddlePointLinearSolver_test
    --matrix-name Assembly_PolyMAC_4_vp
    --matrix-type AssemblyMesh
    --n-proc 2
    --data ${TEST_DATA_DIR}
    --result-path ${TEST_RESULT_DIR}
    --tmp-path ${TEST_TMP_PATH}
)


#python3 ../SaddlePointLinearSolver_SRC/scripts/run_one_test.py \
#    --test-id test \
#    --executable /home/marwane/Desktop/CEA/projet/build/SaddlePointLinearSolver_test \
#    --matrix-name Assembly_PolyMAC_1_vp \
#    --matrix-type AssemblyMesh \
#    --n-proc 1 \
#    --data ../SaddlePointLinearSolver_SRC/tests/data/ \
#    --result-path ../SaddlePointLinearSolver_SRC/tests/results \
#    --tmp-path tmp/output.json