 add_test(NAME testPythonSaddlePointLinearSolver_2x2_seq_Assembly_1                 COMMAND "${Python_EXECUTABLE}" "./testPythonSaddlePointLinearSolver_2x2_seq" "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/AssemblyMesh/Assembly_PolyMAC_1_vp.petsc" "-nU" "20344" "-nP" "9552" )

