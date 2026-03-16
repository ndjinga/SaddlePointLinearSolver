# Generated tests with generate_tests.py script from ${CMAKE_SOURCE_DIR}/tests/tests_parameters.csv

enable_testing()

set(TEST_DATA_DIR ${CMAKE_SOURCE_DIR}/tests/data)
set(TEST_RESULT_DIR ${CMAKE_SOURCE_DIR}/tests/test_results)
set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_2DCartesian_Cartesian_PolyMAC_1_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/2DCartesian/Cartesian_PolyMAC_1_vp.petsc\" \"-nU\" \"40\" \"-nP\" \"16\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Cartesian_PolyMAC_1_vp \
      --matrix-type 2DCartesian \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_2DCartesian_Cartesian_PolyMAC_2_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/2DCartesian/Cartesian_PolyMAC_2_vp.petsc\" \"-nU\" \"144\" \"-nP\" \"64\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Cartesian_PolyMAC_2_vp \
      --matrix-type 2DCartesian \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_2DCartesian_Cartesian_PolyMAC_3_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/2DCartesian/Cartesian_PolyMAC_3_vp.petsc\" \"-nU\" \"544\" \"-nP\" \"256\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Cartesian_PolyMAC_3_vp \
      --matrix-type 2DCartesian \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_2DCartesian_Cartesian_PolyMAC_4_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/2DCartesian/Cartesian_PolyMAC_4_vp.petsc\" \"-nU\" \"2112\" \"-nP\" \"1024\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Cartesian_PolyMAC_4_vp \
      --matrix-type 2DCartesian \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_2DCartesian_Cartesian_PolyMAC_5_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/2DCartesian/Cartesian_PolyMAC_5_vp.petsc\" \"-nU\" \"8320\" \"-nP\" \"4096\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Cartesian_PolyMAC_5_vp \
      --matrix-type 2DCartesian \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_2DKershaw_Kershaw2D_PolyMAC_1_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/2DKershaw/Kershaw2D_PolyMAC_1_vp.petsc\" \"-nU\" \"612\" \"-nP\" \"289\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Kershaw2D_PolyMAC_1_vp \
      --matrix-type 2DKershaw \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_2DKershaw_Kershaw2D_PolyMAC_2_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/2DKershaw/Kershaw2D_PolyMAC_2_vp.petsc\" \"-nU\" \"2380\" \"-nP\" \"1156\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Kershaw2D_PolyMAC_2_vp \
      --matrix-type 2DKershaw \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_2DKershaw_Kershaw2D_PolyMAC_3_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/2DKershaw/Kershaw2D_PolyMAC_3_vp.petsc\" \"-nU\" \"5304\" \"-nP\" \"2601\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Kershaw2D_PolyMAC_3_vp \
      --matrix-type 2DKershaw \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_2DKershaw_Kershaw2D_PolyMAC_4_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/2DKershaw/Kershaw2D_PolyMAC_4_vp.petsc\" \"-nU\" \"9384\" \"-nP\" \"4624\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Kershaw2D_PolyMAC_4_vp \
      --matrix-type 2DKershaw \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_2DPolygons_Polygons_PolyMAC_1_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/2DPolygons/Polygons_PolyMAC_1_vp.petsc\" \"-nU\" \"100\" \"-nP\" \"29\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Polygons_PolyMAC_1_vp \
      --matrix-type 2DPolygons \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_2DPolygons_Polygons_PolyMAC_2_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/2DPolygons/Polygons_PolyMAC_2_vp.petsc\" \"-nU\" \"426\" \"-nP\" \"131\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Polygons_PolyMAC_2_vp \
      --matrix-type 2DPolygons \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_2DPolygons_Polygons_PolyMAC_3_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/2DPolygons/Polygons_PolyMAC_3_vp.petsc\" \"-nU\" \"1591\" \"-nP\" \"506\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Polygons_PolyMAC_3_vp \
      --matrix-type 2DPolygons \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_2DPolygons_Polygons_PolyMAC_4_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/2DPolygons/Polygons_PolyMAC_4_vp.petsc\" \"-nU\" \"10123\" \"-nP\" \"3310\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Polygons_PolyMAC_4_vp \
      --matrix-type 2DPolygons \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_2DPolygons_Polygons_PolyMAC_5_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/2DPolygons/Polygons_PolyMAC_5_vp.petsc\" \"-nU\" \"39798\" \"-nP\" \"13135\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Polygons_PolyMAC_5_vp \
      --matrix-type 2DPolygons \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_2DQuadrangles_Quadrangles_PolyMAC_1_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/2DQuadrangles/Quadrangles_PolyMAC_1_vp.petsc\" \"-nU\" \"40\" \"-nP\" \"16\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Quadrangles_PolyMAC_1_vp \
      --matrix-type 2DQuadrangles \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_2DQuadrangles_Quadrangles_PolyMAC_2_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/2DQuadrangles/Quadrangles_PolyMAC_2_vp.petsc\" \"-nU\" \"144\" \"-nP\" \"64\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Quadrangles_PolyMAC_2_vp \
      --matrix-type 2DQuadrangles \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_2DQuadrangles_Quadrangles_PolyMAC_3_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/2DQuadrangles/Quadrangles_PolyMAC_3_vp.petsc\" \"-nU\" \"544\" \"-nP\" \"256\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Quadrangles_PolyMAC_3_vp \
      --matrix-type 2DQuadrangles \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_2DQuadrangles_Quadrangles_PolyMAC_4_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/2DQuadrangles/Quadrangles_PolyMAC_4_vp.petsc\" \"-nU\" \"2112\" \"-nP\" \"1024\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Quadrangles_PolyMAC_4_vp \
      --matrix-type 2DQuadrangles \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_2DQuadrangles_Quadrangles_PolyMAC_5_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/2DQuadrangles/Quadrangles_PolyMAC_5_vp.petsc\" \"-nU\" \"8320\" \"-nP\" \"4096\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Quadrangles_PolyMAC_5_vp \
      --matrix-type 2DQuadrangles \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_2DQuadrangles_Quadrangles_PolyMAC_6_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/2DQuadrangles/Quadrangles_PolyMAC_6_vp.petsc\" \"-nU\" \"33024\" \"-nP\" \"16384\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Quadrangles_PolyMAC_6_vp \
      --matrix-type 2DQuadrangles \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_2DQuadrangles_Quadrangles_PolyMAC_7_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/2DQuadrangles/Quadrangles_PolyMAC_7_vp.petsc\" \"-nU\" \"131584\" \"-nP\" \"65536\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Quadrangles_PolyMAC_7_vp \
      --matrix-type 2DQuadrangles \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_2DTriangles_Triangles_PolyMAC_1_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/2DTriangles/Triangles_PolyMAC_1_vp.petsc\" \"-nU\" \"68\" \"-nP\" \"40\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Triangles_PolyMAC_1_vp \
      --matrix-type 2DTriangles \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_2DTriangles_Triangles_PolyMAC_2_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/2DTriangles/Triangles_PolyMAC_2_vp.petsc\" \"-nU\" \"354\" \"-nP\" \"224\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Triangles_PolyMAC_2_vp \
      --matrix-type 2DTriangles \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_2DTriangles_Triangles_PolyMAC_3_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/2DTriangles/Triangles_PolyMAC_3_vp.petsc\" \"-nU\" \"1439\" \"-nP\" \"934\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Triangles_PolyMAC_3_vp \
      --matrix-type 2DTriangles \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_2DTriangles_Triangles_PolyMAC_4_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/2DTriangles/Triangles_PolyMAC_4_vp.petsc\" \"-nU\" \"9731\" \"-nP\" \"6422\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Triangles_PolyMAC_4_vp \
      --matrix-type 2DTriangles \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_2DTriangles_Triangles_PolyMAC_5_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/2DTriangles/Triangles_PolyMAC_5_vp.petsc\" \"-nU\" \"39006\" \"-nP\" \"25872\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Triangles_PolyMAC_5_vp \
      --matrix-type 2DTriangles \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_2DTriangles_Triangles_PolyMAC_6_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/2DTriangles/Triangles_PolyMAC_6_vp.petsc\" \"-nU\" \"157028\" \"-nP\" \"104420\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Triangles_PolyMAC_6_vp \
      --matrix-type 2DTriangles \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DAssemblyMesh_Assembly_PolyMAC_1_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DAssemblyMesh/Assembly_PolyMAC_1_vp.petsc\" \"-nU\" \"20344\" \"-nP\" \"9552\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Assembly_PolyMAC_1_vp \
      --matrix-type 3DAssemblyMesh \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DAssemblyMesh_Assembly_PolyMAC_2_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DAssemblyMesh/Assembly_PolyMAC_2_vp.petsc\" \"-nU\" \"20799\" \"-nP\" \"9614\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Assembly_PolyMAC_2_vp \
      --matrix-type 3DAssemblyMesh \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DAssemblyMesh_Assembly_PolyMAC_3_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DAssemblyMesh/Assembly_PolyMAC_3_vp.petsc\" \"-nU\" \"40858\" \"-nP\" \"18122\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Assembly_PolyMAC_3_vp \
      --matrix-type 3DAssemblyMesh \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DAssemblyMesh_Assembly_PolyMAC_4_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DAssemblyMesh/Assembly_PolyMAC_4_vp.petsc\" \"-nU\" \"114220\" \"-nP\" \"49571\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Assembly_PolyMAC_4_vp \
      --matrix-type 3DAssemblyMesh \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DAssemblyMesh_Assembly_PolyMAC_5_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DAssemblyMesh/Assembly_PolyMAC_5_vp.petsc\" \"-nU\" \"303660\" \"-nP\" \"131979\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Assembly_PolyMAC_5_vp \
      --matrix-type 3DAssemblyMesh \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DCheckerBoard_CheckerBoard_PolyMAC_1_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DCheckerBoard/CheckerBoard_PolyMAC_1_vp.petsc\" \"-nU\" \"156\" \"-nP\" \"36\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name CheckerBoard_PolyMAC_1_vp \
      --matrix-type 3DCheckerBoard \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DCheckerBoard_CheckerBoard_PolyMAC_2_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DCheckerBoard/CheckerBoard_PolyMAC_2_vp.petsc\" \"-nU\" \"1200\" \"-nP\" \"288\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name CheckerBoard_PolyMAC_2_vp \
      --matrix-type 3DCheckerBoard \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DCheckerBoard_CheckerBoard_PolyMAC_3_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DCheckerBoard/CheckerBoard_PolyMAC_3_vp.petsc\" \"-nU\" \"9408\" \"-nP\" \"2304\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name CheckerBoard_PolyMAC_3_vp \
      --matrix-type 3DCheckerBoard \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DCheckerBoard_CheckerBoard_PolyMAC_4_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DCheckerBoard/CheckerBoard_PolyMAC_4_vp.petsc\" \"-nU\" \"74496\" \"-nP\" \"18432\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name CheckerBoard_PolyMAC_4_vp \
      --matrix-type 3DCheckerBoard \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DHexa_Hexa_PolyMAC_1_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DHexa/Hexa_PolyMAC_1_vp.petsc\" \"-nU\" \"36\" \"-nP\" \"8\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Hexa_PolyMAC_1_vp \
      --matrix-type 3DHexa \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DHexa_Hexa_PolyMAC_2_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DHexa/Hexa_PolyMAC_2_vp.petsc\" \"-nU\" \"240\" \"-nP\" \"64\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Hexa_PolyMAC_2_vp \
      --matrix-type 3DHexa \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DHexa_Hexa_PolyMAC_3_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DHexa/Hexa_PolyMAC_3_vp.petsc\" \"-nU\" \"1728\" \"-nP\" \"512\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Hexa_PolyMAC_3_vp \
      --matrix-type 3DHexa \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DHexa_Hexa_PolyMAC_4_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DHexa/Hexa_PolyMAC_4_vp.petsc\" \"-nU\" \"13056\" \"-nP\" \"4096\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Hexa_PolyMAC_4_vp \
      --matrix-type 3DHexa \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DKershaw_Kershaw_PolyMAC_1_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DKershaw/Kershaw_PolyMAC_1_vp.petsc\" \"-nU\" \"1728\" \"-nP\" \"512\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Kershaw_PolyMAC_1_vp \
      --matrix-type 3DKershaw \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DKershaw_Kershaw_PolyMAC_2_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DKershaw/Kershaw_PolyMAC_2_vp.petsc\" \"-nU\" \"13056\" \"-nP\" \"4096\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Kershaw_PolyMAC_2_vp \
      --matrix-type 3DKershaw \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DKershaw_Kershaw_PolyMAC_3_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DKershaw/Kershaw_PolyMAC_3_vp.petsc\" \"-nU\" \"101376\" \"-nP\" \"32768\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Kershaw_PolyMAC_3_vp \
      --matrix-type 3DKershaw \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DLocRaf_LocRaf_PolyMAC_1_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DLocRaf/LocRaf_PolyMAC_1_vp.petsc\" \"-nU\" \"93\" \"-nP\" \"22\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name LocRaf_PolyMAC_1_vp \
      --matrix-type 3DLocRaf \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DLocRaf_LocRaf_PolyMAC_2_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DLocRaf/LocRaf_PolyMAC_2_vp.petsc\" \"-nU\" \"636\" \"-nP\" \"176\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name LocRaf_PolyMAC_2_vp \
      --matrix-type 3DLocRaf \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DLocRaf_LocRaf_PolyMAC_3_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DLocRaf/LocRaf_PolyMAC_3_vp.petsc\" \"-nU\" \"4656\" \"-nP\" \"1408\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name LocRaf_PolyMAC_3_vp \
      --matrix-type 3DLocRaf \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DLocRaf_LocRaf_PolyMAC_4_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DLocRaf/LocRaf_PolyMAC_4_vp.petsc\" \"-nU\" \"35520\" \"-nP\" \"11264\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name LocRaf_PolyMAC_4_vp \
      --matrix-type 3DLocRaf \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DLocRaf_LocRaf_PolyMAC_5_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DLocRaf/LocRaf_PolyMAC_5_vp.petsc\" \"-nU\" \"277248\" \"-nP\" \"90112\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name LocRaf_PolyMAC_5_vp \
      --matrix-type 3DLocRaf \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DPrism_Prism_PolyMAC_1_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DPrism/Prism_PolyMAC_1_vp.petsc\" \"-nU\" \"5400\" \"-nP\" \"2000\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Prism_PolyMAC_1_vp \
      --matrix-type 3DPrism \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DPrism_Prism_PolyMAC_2_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DPrism/Prism_PolyMAC_2_vp.petsc\" \"-nU\" \"41600\" \"-nP\" \"16000\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Prism_PolyMAC_2_vp \
      --matrix-type 3DPrism \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DPrism_Prism_PolyMAC_3_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DPrism/Prism_PolyMAC_3_vp.petsc\" \"-nU\" \"138600\" \"-nP\" \"54000\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Prism_PolyMAC_3_vp \
      --matrix-type 3DPrism \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DPrism_Prism_PolyMAC_4_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DPrism/Prism_PolyMAC_4_vp.petsc\" \"-nU\" \"326400\" \"-nP\" \"128000\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Prism_PolyMAC_4_vp \
      --matrix-type 3DPrism \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DRandom_Random_PolyMAC_1_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DRandom/Random_PolyMAC_1_vp.petsc\" \"-nU\" \"480\" \"-nP\" \"64\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Random_PolyMAC_1_vp \
      --matrix-type 3DRandom \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DRandom_Random_PolyMAC_2_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DRandom/Random_PolyMAC_2_vp.petsc\" \"-nU\" \"3456\" \"-nP\" \"512\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Random_PolyMAC_2_vp \
      --matrix-type 3DRandom \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DRandom_Random_PolyMAC_3_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DRandom/Random_PolyMAC_3_vp.petsc\" \"-nU\" \"26112\" \"-nP\" \"4096\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Random_PolyMAC_3_vp \
      --matrix-type 3DRandom \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DTetra_Tetra_PolyMAC_1_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DTetra/Tetra_PolyMAC_1_vp.petsc\" \"-nU\" \"500\" \"-nP\" \"215\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Tetra_PolyMAC_1_vp \
      --matrix-type 3DTetra \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DTetra_Tetra_PolyMAC_2_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DTetra/Tetra_PolyMAC_2_vp.petsc\" \"-nU\" \"4308\" \"-nP\" \"2003\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Tetra_PolyMAC_2_vp \
      --matrix-type 3DTetra \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DTetra_Tetra_PolyMAC_3_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DTetra/Tetra_PolyMAC_3_vp.petsc\" \"-nU\" \"8248\" \"-nP\" \"3898\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Tetra_PolyMAC_3_vp \
      --matrix-type 3DTetra \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DTetra_Tetra_PolyMAC_4_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DTetra/Tetra_PolyMAC_4_vp.petsc\" \"-nU\" \"16148\" \"-nP\" \"7711\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Tetra_PolyMAC_4_vp \
      --matrix-type 3DTetra \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DVoronoi_Voronoi_PolyMAC_1_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DVoronoi/Voronoi_PolyMAC_1_vp.petsc\" \"-nU\" \"172\" \"-nP\" \"29\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Voronoi_PolyMAC_1_vp \
      --matrix-type 3DVoronoi \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DVoronoi_Voronoi_PolyMAC_2_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DVoronoi/Voronoi_PolyMAC_2_vp.petsc\" \"-nU\" \"402\" \"-nP\" \"66\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Voronoi_PolyMAC_2_vp \
      --matrix-type 3DVoronoi \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DVoronoi_Voronoi_PolyMAC_3_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DVoronoi/Voronoi_PolyMAC_3_vp.petsc\" \"-nU\" \"811\" \"-nP\" \"130\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Voronoi_PolyMAC_3_vp \
      --matrix-type 3DVoronoi \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DVoronoi_Voronoi_PolyMAC_4_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DVoronoi/Voronoi_PolyMAC_4_vp.petsc\" \"-nU\" \"1452\" \"-nP\" \"228\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Voronoi_PolyMAC_4_vp \
      --matrix-type 3DVoronoi \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

set(TEST_ID complete_testSaddlePointLinearSolver_2x2_par_instrumentation_3DVoronoi_Voronoi_PolyMAC_5_vp_nproc1)
set(TMP_FILE ${TMP_DIR}/tmp_output_${TEST_ID}.json)
add_test(
  NAME ${TEST_ID}
  COMMAND bash -c "
    \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \"-f0\" \"${CMAKE_SOURCE_DIR}/tests/data/matrices/3DVoronoi/Voronoi_PolyMAC_5_vp.petsc\" \"-nU\" \"2376\" \"-nP\" \"356\" \"-tmp_file\" \"${TMP_FILE}\"; \
    python3 ${CMAKE_SOURCE_DIR}/scripts/collect_test_data.py \
      --test-id \"${TEST_ID}\" \
      --executable \"${CMAKE_BINARY_DIR}/testSaddlePointLinearSolver_2x2_par_instrumentation\" \
      --matrix-name Voronoi_PolyMAC_5_vp \
      --matrix-type 3DVoronoi \
      --n-proc 1 \
      --option \"\" \
      --data-dir \"${TEST_DATA_DIR}\" \
      --test-results-dir \"${TEST_RESULT_DIR}\" \
      --tmp-file \"${TMP_FILE}\"
  "
)

