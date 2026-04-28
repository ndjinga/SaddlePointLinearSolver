cmake_minimum_required(VERSION 3.12.0)

project("SaddlePointLinearSolver")
set(PROJECT_BRIEF "A preconditioner for linear systems with 2x2 block matrices")

list (APPEND CMAKE_MODULE_PATH "${SaddlePointLinearSolver_SOURCE_DIR}/cmake")

option (SaddlePointLinearSolver_WITH_PYTHON     "Build Python interface" ON)
option (SaddlePointLinearSolver_WITH_TESTS      "Build and run example tests" ON)
option (SaddlePointLinearSolver_WITH_MPI        "Generate a parallel version" OFF)

#Path to installed libraries
set (PYTHON_ROOT_DIR      OFF CACHE STRING "Python library path" )
set (PETSC_DIR            OFF CACHE STRING "PETSc library path" )
set (PETSC_ARCH           OFF CACHE STRING "PETSc architecture" )
set (MPI_HOME             OFF CACHE STRING "MPI library path" )

find_package (PETSc 3.4 REQUIRED)

if( SaddlePointLinearSolver_WITH_MPI )
  message(STATUS "SaddlePointLinearSolver requested with MPI library. Searching MPI library.")
  if   ( NOT MPI_HOME)
    set( MPI_HOME ${PETSC_DIR}/${PETSC_ARCH} )
  endif( NOT MPI_HOME)
  find_package(MPI REQUIRED)
endif( SaddlePointLinearSolver_WITH_MPI )

if( SaddlePointLinearSolver_WITH_PYTHON )
  message(STATUS "SaddlePointLinearSolver requested with PYTHON library. Searching PYTHON library.")
  find_package(Python ${PYTHON2OR3} REQUIRED COMPONENTS Interpreter Development )
  SET(PYTHON_EXECUTABLE ${Python_EXECUTABLE})
 # find_package (SWIG 3.0 REQUIRED)
 # add_subdirectory(swig)
endif( SaddlePointLinearSolver_WITH_PYTHON )

set(MY_HEADERS
src
src/PCSHELL/
)

add_subdirectory(src)

if( SaddlePointLinearSolver_WITH_TESTS )
 enable_testing ()

set(MY_TEST_SOURCES
 tests/src/testSaddlePointLinearSolver_initial_seq.c
 tests/src/testSaddlePointLinearSolver_2x2_APIRight.c
 tests/src/testSaddlePointLinearSolver_2x2_APIRightWithILU.c
 tests/src/testSaddlePointLinearSolver_2x2_APILeft.c
 tests/src/testSaddlePointLinearSolver_2x2_APILeftRight.c
 tests/src/testSaddlePointLinearSolver_2x2_APISchur.c
 tests/src/testSaddlePointLinearSolver_3x3_APISchur.c
 tests/src/testSaddlePointLinearSolver_2x2_PCSHELL_Schur.c
 tests/src/testSaddlePointLinearSolver_2x2_APISchurHypre.c
 tests/src/testSaddlePointLinearSolver_2x2_par.c
 tests/src/testSaddlePointLinearSolver_2x2_par_instrumentation.c
 #tests/src/testSaddlePointLinearSolver_nxn_par.c
 #tests/src/testSaddlePointLinearSolver_2x2_Shat.c
 )

 # build test main exe
 add_executable(testSaddlePointLinearSolver_initial_seq                  tests/src/testSaddlePointLinearSolver_initial_seq.c)
 add_executable(testSaddlePointLinearSolver_initial_Shat                 tests/src/testSaddlePointLinearSolver_initial_Shat.c)
 add_executable(testSaddlePointLinearSolver_2x2_APIRight                 tests/src/testSaddlePointLinearSolver_2x2_API_Right.c)
 add_executable(testSaddlePointLinearSolver_2x2_APIRightWithILU          tests/src/testSaddlePointLinearSolver_2x2_API_RightWithILU.c)
 add_executable(testSaddlePointLinearSolver_2x2_APILeft                  tests/src/testSaddlePointLinearSolver_2x2_API_Left.c)
 add_executable(testSaddlePointLinearSolver_2x2_APILeftRight             tests/src/testSaddlePointLinearSolver_2x2_API_LeftRight.c)
 add_executable(testSaddlePointLinearSolver_2x2_APISchur                 tests/src/testSaddlePointLinearSolver_2x2_API_Schur.c)
 add_executable(testSaddlePointLinearSolver_3x3_APISchur                 tests/src/testSaddlePointLinearSolver_3x3_API_Schur.c)
 add_executable(testSaddlePointLinearSolver_2x2_PCSHELL_Schur            tests/src/testSaddlePointLinearSolver_2x2_PCSHELL_Schur.c)
 add_executable(testSaddlePointLinearSolver_2x2_APISchurHypre            tests/src/testSaddlePointLinearSolver_2x2_API_Schur_Hypre.c)
 add_executable(testSaddlePointLinearSolver_2x2_par                      tests/src/testSaddlePointLinearSolver_2x2_par.c)
 add_executable(testSaddlePointLinearSolver_2x2_par_instrumentation      tests/src/testSaddlePointLinearSolver_2x2_par_instrumentation.c)
 target_include_directories(testSaddlePointLinearSolver_initial_seq                  PRIVATE ${PETSC_INCLUDES} )
 target_include_directories(testSaddlePointLinearSolver_initial_Shat                 PRIVATE ${PETSC_INCLUDES} )
 target_include_directories(testSaddlePointLinearSolver_2x2_APIRight                 PRIVATE ${PETSC_INCLUDES}  ${MY_HEADERS})
 target_include_directories(testSaddlePointLinearSolver_2x2_APIRightWithILU          PRIVATE ${PETSC_INCLUDES}  ${MY_HEADERS})
 target_include_directories(testSaddlePointLinearSolver_2x2_APILeft                  PRIVATE ${PETSC_INCLUDES}  ${MY_HEADERS})
 target_include_directories(testSaddlePointLinearSolver_2x2_APILeftRight             PRIVATE ${PETSC_INCLUDES}  ${MY_HEADERS})
 target_include_directories(testSaddlePointLinearSolver_2x2_APISchur                 PRIVATE ${PETSC_INCLUDES}  ${MY_HEADERS})
 target_include_directories(testSaddlePointLinearSolver_3x3_APISchur                 PRIVATE ${PETSC_INCLUDES}  ${MY_HEADERS})
 target_include_directories(testSaddlePointLinearSolver_2x2_APISchurHypre            PRIVATE ${PETSC_INCLUDES}  ${MY_HEADERS})
 target_include_directories(testSaddlePointLinearSolver_2x2_PCSHELL_Schur            PRIVATE ${PETSC_INCLUDES}  ${MY_HEADERS})
 target_include_directories(testSaddlePointLinearSolver_2x2_par                      PRIVATE ${PETSC_INCLUDES} )
 target_include_directories(testSaddlePointLinearSolver_2x2_par_instrumentation      PRIVATE ${PETSC_INCLUDES} )

 target_link_libraries(testSaddlePointLinearSolver_2x2_APIRight           SaddlePointLinearSolverLib )
 target_link_libraries(testSaddlePointLinearSolver_2x2_APIRightWithILU    SaddlePointLinearSolverLib )
 target_link_libraries(testSaddlePointLinearSolver_2x2_APILeft            SaddlePointLinearSolverLib )
 target_link_libraries(testSaddlePointLinearSolver_2x2_APILeftRight       SaddlePointLinearSolverLib )
 target_link_libraries(testSaddlePointLinearSolver_2x2_APISchur           SaddlePointLinearSolverLib )
 target_link_libraries(testSaddlePointLinearSolver_3x3_APISchur           SaddlePointLinearSolverLib )
 target_link_libraries(testSaddlePointLinearSolver_2x2_APISchurHypre      SaddlePointLinearSolverLib )
 target_link_libraries(testSaddlePointLinearSolver_2x2_PCSHELL_Schur      SaddlePointLinearSolverLib )

 target_link_libraries(testSaddlePointLinearSolver_initial_seq                  ${PETSC_LIBRARIES} ${MPI_HOME}/lib/libmpi.so)
 target_link_libraries(testSaddlePointLinearSolver_initial_Shat                 ${PETSC_LIBRARIES} ${MPI_HOME}/lib/libmpi.so)
 target_link_libraries(testSaddlePointLinearSolver_2x2_APIRight                 ${PETSC_LIBRARIES} ${MPI_HOME}/lib/libmpi.so)
 target_link_libraries(testSaddlePointLinearSolver_2x2_APIRightWithILU          ${PETSC_LIBRARIES} ${MPI_HOME}/lib/libmpi.so)
 target_link_libraries(testSaddlePointLinearSolver_2x2_APILeft                  ${PETSC_LIBRARIES} ${MPI_HOME}/lib/libmpi.so)
 target_link_libraries(testSaddlePointLinearSolver_2x2_APILeftRight             ${PETSC_LIBRARIES} ${MPI_HOME}/lib/libmpi.so)
 target_link_libraries(testSaddlePointLinearSolver_2x2_APISchur                 ${PETSC_LIBRARIES} ${MPI_HOME}/lib/libmpi.so)
 target_link_libraries(testSaddlePointLinearSolver_3x3_APISchur                 ${PETSC_LIBRARIES} ${MPI_HOME}/lib/libmpi.so)
 target_link_libraries(testSaddlePointLinearSolver_2x2_APISchurHypre            ${PETSC_LIBRARIES} ${MPI_HOME}/lib/libmpi.so)
 target_link_libraries(testSaddlePointLinearSolver_2x2_PCSHELL_Schur            ${PETSC_LIBRARIES} ${MPI_HOME}/lib/libmpi.so)
 target_link_libraries(testSaddlePointLinearSolver_2x2_par                      ${PETSC_LIBRARIES} ${MPI_HOME}/lib/libmpi.so -lm)
 target_link_libraries(testSaddlePointLinearSolver_2x2_par_instrumentation      ${PETSC_LIBRARIES} ${MPI_HOME}/lib/libmpi.so -lm)

add_test(NAME testSaddlePointLinearSolver_initial_seq_Assembly_1                 COMMAND "./testSaddlePointLinearSolver_initial_seq"                 "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/3DAssemblyMesh/Assembly_PolyMAC_1_vp.petsc" "-nU" "20344" "-nP" "9552" "-pc_fieldsplit_type" "schur" "-pc_fieldsplit_schur_fact_type" "lower" "-fieldsplit_0_ksp_type" "gmres" "-fieldsplit_0_pc_type" "gamg" "-fieldsplit_0_pc_gamg_type" "agg" "-fieldsplit_1_ksp_type" "preonly" "-fieldsplit_1_pc_type" "lu")
 set_property (TEST testSaddlePointLinearSolver_initial_seq_Assembly_1 PROPERTY LABELS "C++ initial_seq PC_COMPOSITE Assembly Assembly_1")     
add_test(NAME testSaddlePointLinearSolver_initial_Shat_Assembly_1            COMMAND "./testSaddlePointLinearSolver_initial_Shat"            "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/3DAssemblyMesh/Assembly_PolyMAC_1_vp.petsc" "-nU" "20344" "-nP" "9552" "-pc_fieldsplit_type" "schur" "-pc_fieldsplit_schur_fact_type" "lower" "-fieldsplit_0_ksp_type" "gmres" "-fieldsplit_0_pc_type" "gamg" "-fieldsplit_0_pc_gamg_type" "agg" "-fieldsplit_1_ksp_type" "preonly" "-fieldsplit_1_pc_type" "lu" "-emptyPressureRHS" "no")
 set_property (TEST testSaddlePointLinearSolver_initial_Shat_Assembly_1 PROPERTY LABELS "C++ initial PC_SCHUR Shat Assembly Assembly_1")     
add_test(NAME testSaddlePointLinearSolver_initial_Shat_Assembly_1_emptyPressureRHS            COMMAND "./testSaddlePointLinearSolver_initial_Shat"            "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/3DAssemblyMesh/Assembly_PolyMAC_1_vp.petsc" "-nU" "20344" "-nP" "9552" "-pc_fieldsplit_type" "schur" "-pc_fieldsplit_schur_fact_type" "lower" "-fieldsplit_0_ksp_type" "gmres" "-fieldsplit_0_pc_type" "gamg" "-fieldsplit_0_pc_gamg_type" "agg" "-fieldsplit_1_ksp_type" "preonly" "-fieldsplit_1_pc_type" "lu" "-emptyPressureRHS" "yes")
 set_property (TEST testSaddlePointLinearSolver_initial_Shat_Assembly_1_emptyPressureRHS PROPERTY LABELS "C++ initial PC_SCHUR Shat Assembly Assembly_1 emptyPressureRHS")     
add_test(NAME testSaddlePointLinearSolver_2x2_APIRight_Cartesian_1               COMMAND "./testSaddlePointLinearSolver_2x2_APIRight"                 "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/2DCartesian/Cartesian_PolyMAC_1_vp.petsc" "-nU" "40" "-nP" "16")
 set_property (TEST testSaddlePointLinearSolver_2x2_APIRight_Cartesian_1 PROPERTY LABELS "C++ 2x2 API Right PC_COMPOSITE Cartesian Cartesian_1")     
add_test(NAME testSaddlePointLinearSolver_2x2_APILeft_Cartesian_1                COMMAND "./testSaddlePointLinearSolver_2x2_APILeft"                 "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/2DCartesian/Cartesian_PolyMAC_1_vp.petsc" "-nU" "40" "-nP" "16")
 set_property (TEST testSaddlePointLinearSolver_2x2_APILeft_Cartesian_1 PROPERTY LABELS "C++ 2x2 API Left PC_COMPOSITE Cartesian Cartesian_1")     
add_test(NAME testSaddlePointLinearSolver_2x2_APILeftRight_Cartesian_1           COMMAND "./testSaddlePointLinearSolver_2x2_APILeftRight"                 "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/2DCartesian/Cartesian_PolyMAC_1_vp.petsc" "-nU" "40" "-nP" "16")
 set_property (TEST testSaddlePointLinearSolver_2x2_APILeftRight_Cartesian_1 PROPERTY LABELS "C++ 2x2 API LeftRight PC_COMPOSITE Cartesian Cartesian_1")     
add_test(NAME testSaddlePointLinearSolver_2x2_APIRightWithILU_Cartesian_1               COMMAND "./testSaddlePointLinearSolver_2x2_APIRightWithILU"                 "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/2DCartesian/Cartesian_PolyMAC_1_vp.petsc" "-nU" "40" "-nP" "16")
 set_property (TEST testSaddlePointLinearSolver_2x2_APIRightWithILU_Cartesian_1 PROPERTY LABELS "C++ 2x2 API RightWithILU PC_COMPOSITE Cartesian Cartesian_1")     
add_test(NAME testSaddlePointLinearSolver_2x2_APISchur_Cartesian_1                COMMAND "./testSaddlePointLinearSolver_2x2_APISchur"                 "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/2DCartesian/Cartesian_PolyMAC_1_vp.petsc" "-nU" "40" "-nP" "16")
 set_property (TEST testSaddlePointLinearSolver_2x2_APISchur_Cartesian_1 PROPERTY LABELS "C++ 2x2 API PCFIELDSPLIT Schur Cartesian Cartesian_1")     
add_test(NAME testSaddlePointLinearSolver_3x3_APISchur_Cartesian_1                COMMAND "./testSaddlePointLinearSolver_3x3_APISchur"                 "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/2DCartesian/Cartesian_PolyMAC_1.petsc" "-nU" "40" "-nP" "16" "-nOmega" "25")
 set_property (TEST testSaddlePointLinearSolver_3x3_APISchur_Cartesian_1 PROPERTY LABELS "C++ 3x3 API PCFIELDSPLIT Schur Cartesian Cartesian_1")     
add_test(NAME testSaddlePointLinearSolver_2x2_PCSHELL_Schur_Cartesian_1                COMMAND "./testSaddlePointLinearSolver_2x2_PCSHELL_Schur"                 "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/2DCartesian/Cartesian_PolyMAC_1_vp.petsc" "-nU" "40" "-nP" "16")
 set_property (TEST testSaddlePointLinearSolver_2x2_PCSHELL_Schur_Cartesian_1 PROPERTY LABELS "C++ 2x2 API PCSHELL Schur Cartesian Cartesian_1")     
add_test(NAME testSaddlePointLinearSolver_2x2_APIRight_Assembly_1                COMMAND "./testSaddlePointLinearSolver_2x2_APIRight"                 "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/3DAssemblyMesh/Assembly_PolyMAC_1_vp.petsc" "-nU" "20344" "-nP" "9552" )
 set_property (TEST testSaddlePointLinearSolver_2x2_APIRight_Assembly_1 PROPERTY LABELS "C++ 2x2 API Right PC_COMPOSITE Assembly Assembly_1")     
add_test(NAME testSaddlePointLinearSolver_2x2_APILeft_Assembly_1                 COMMAND "./testSaddlePointLinearSolver_2x2_APILeft"                 "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/3DAssemblyMesh/Assembly_PolyMAC_1_vp.petsc" "-nU" "20344" "-nP" "9552" )
 set_property (TEST testSaddlePointLinearSolver_2x2_APILeft_Assembly_1 PROPERTY LABELS "C++ 2x2 API Left PC_COMPOSITE Assembly Assembly_1")     
add_test(NAME testSaddlePointLinearSolver_2x2_APILeftRight_Assembly_1            COMMAND "./testSaddlePointLinearSolver_2x2_APILeftRight"                 "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/3DAssemblyMesh/Assembly_PolyMAC_1_vp.petsc" "-nU" "20344" "-nP" "9552" )
 set_property (TEST testSaddlePointLinearSolver_2x2_APILeftRight_Assembly_1 PROPERTY LABELS "C++ 2x2 API LeftRight PC_COMPOSITE Assembly Assembly_1")     
add_test(NAME testSaddlePointLinearSolver_2x2_APIRightWithILU_Assembly_1         COMMAND "./testSaddlePointLinearSolver_2x2_APIRightWithILU"                 "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/3DAssemblyMesh/Assembly_PolyMAC_1_vp.petsc" "-nU" "20344" "-nP" "9552")
 set_property (TEST testSaddlePointLinearSolver_2x2_APIRightWithILU_Assembly_1 PROPERTY LABELS "C++ 2x2 API RightWithILU PC_COMPOSITE Assembly Assembly_1")     
add_test(NAME testSaddlePointLinearSolver_2x2_APISchur_Assembly_1                COMMAND "./testSaddlePointLinearSolver_2x2_APISchur"                 "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/3DAssemblyMesh/Assembly_PolyMAC_1_vp.petsc" "-nU" "20344" "-nP" "9552" )
 set_property (TEST testSaddlePointLinearSolver_2x2_APISchur_Assembly_1 PROPERTY LABELS "C++ 2x2 API PCFIELDSPLIT Schur Assembly Assembly_1")     
add_test(NAME testSaddlePointLinearSolver_3x3_APISchur_Assembly_1                COMMAND "./testSaddlePointLinearSolver_3x3_APISchur"                 "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/3DAssemblyMesh/Assembly_PolyMAC_1.petsc" "-nU" "20344" "-nP" "9552" "-nOmega" "12972")
 set_property (TEST testSaddlePointLinearSolver_3x3_APISchur_Assembly_1 PROPERTY LABELS "C++ 3x3 API PCFIELDSPLIT Schur Assembly Assembly_1")     
add_test(NAME testSaddlePointLinearSolver_2x2_PCSHELL_Schur_Assembly_1                COMMAND "./testSaddlePointLinearSolver_2x2_PCSHELL_Schur"                 "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/3DAssemblyMesh/Assembly_PolyMAC_1_vp.petsc" "-nU" "20344" "-nP" "9552" )
 set_property (TEST testSaddlePointLinearSolver_2x2_PCSHELL_Schur_Assembly_1 PROPERTY LABELS "C++ 2x2 API PCSHELL Schur Assembly Assembly_1")     
add_test(NAME testSaddlePointLinearSolver_2x2_APISchurHypre_Assembly_1                COMMAND "./testSaddlePointLinearSolver_2x2_APISchurHypre"                 "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/3DAssemblyMesh/Assembly_PolyMAC_1_vp.petsc" "-nU" "20344" "-nP" "9552" )
 set_property (TEST testSaddlePointLinearSolver_2x2_APISchurHypre_Assembly_1 PROPERTY LABELS "C++ 2x2 API PCFIELDSPLIT Schur Assembly Assembly_1")     
add_test(NAME testSaddlePointLinearSolver_2x2_par_Cartesian_1                 COMMAND "./testSaddlePointLinearSolver_2x2_par"                 "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/2DCartesian/Cartesian_PolyMAC_1_vp.petsc" "-nU" "40" "-nP" "16")
 set_property (TEST testSaddlePointLinearSolver_2x2_par_Cartesian_1 PROPERTY LABELS "C++ 2x2 par1 PC_COMPOSITE Cartesian Cartesian_1")     
add_test(NAME testSaddlePointLinearSolver_2x2_par_Cartesian_3                 COMMAND "./testSaddlePointLinearSolver_2x2_par"                 "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/2DCartesian/Cartesian_PolyMAC_3_vp.petsc" "-nU" "544" "-nP" "256")
 set_property (TEST testSaddlePointLinearSolver_2x2_par_Cartesian_3 PROPERTY LABELS "C++ 2x2 par1 PC_COMPOSITE Cartesian Cartesian_3")     
add_test(NAME testSaddlePointLinearSolver_2x2_par_Cartesian_4                 COMMAND "./testSaddlePointLinearSolver_2x2_par"                 "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/2DCartesian/Cartesian_PolyMAC_4_vp.petsc" "-nU" "2112" "-nP" "1024")
 set_property (TEST testSaddlePointLinearSolver_2x2_par_Cartesian_4 PROPERTY LABELS "C++ 2x2 par1 PC_COMPOSITE Cartesian Cartesian_4")     
add_test(NAME testSaddlePointLinearSolver_2x2_par_Cartesian_5                 COMMAND "./testSaddlePointLinearSolver_2x2_par"                 "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/2DCartesian/Cartesian_PolyMAC_5_vp.petsc" "-nU" "8320" "-nP" "4096")
 set_property (TEST testSaddlePointLinearSolver_2x2_par_Cartesian_5 PROPERTY LABELS "C++ 2x2 par1 PC_COMPOSITE Cartesian Cartesian_5")     
add_test(NAME testSaddlePointLinearSolver_2x2_par_Assembly_1                 COMMAND "./testSaddlePointLinearSolver_2x2_par"                 "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/3DAssemblyMesh/Assembly_PolyMAC_1_vp.petsc" "-nU" "20344" "-nP" "9552")
 set_property (TEST testSaddlePointLinearSolver_2x2_par_Assembly_1 PROPERTY LABELS "C++ 2x2 par1 PC_COMPOSITE Assembly Assembly_1")     
add_test(NAME testSaddlePointLinearSolver_2x2_par_instrumentation_Assembly_1 COMMAND "./testSaddlePointLinearSolver_2x2_par_instrumentation" "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/3DAssemblyMesh/Assembly_PolyMAC_1_vp.petsc" "-nU" "20344" "-nP" "9552")
 set_property (TEST testSaddlePointLinearSolver_2x2_par_instrumentation_Assembly_1 PROPERTY LABELS "C++ 2x2 par1 instrumentation PC_COMPOSITE Assembly Assembly_1")     

if( SaddlePointLinearSolver_WITH_MPI )
 add_test(NAME testSaddlePointLinearSolver_2x2_APIRight_2procs_Assembly_1  COMMAND "${MPIEXEC}" "-n" "2" "./testSaddlePointLinearSolver_2x2_APIRight" "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/3DAssemblyMesh/Assembly_PolyMAC_1_vp.petsc" "-nU" "20344" "-nP" "9552")
 add_test(NAME testSaddlePointLinearSolver_2x2_par_2procs_Cartesian_3                 COMMAND "${MPIEXEC}" "-n" "2" "./testSaddlePointLinearSolver_2x2_par"                 "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/2DCartesian/Cartesian_PolyMAC_3_vp.petsc" "-nU" "544" "-nP" "256")
 add_test(NAME testSaddlePointLinearSolver_2x2_par_2procs_Cartesian_4                 COMMAND "${MPIEXEC}" "-n" "2" "./testSaddlePointLinearSolver_2x2_par"                 "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/2DCartesian/Cartesian_PolyMAC_4_vp.petsc" "-nU" "2112" "-nP" "1024")
 add_test(NAME testSaddlePointLinearSolver_2x2_par_4procs_Cartesian_4                 COMMAND "${MPIEXEC}" "-n" "4" "./testSaddlePointLinearSolver_2x2_par"                 "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/2DCartesian/Cartesian_PolyMAC_4_vp.petsc" "-nU" "2112" "-nP" "1024")
 add_test(NAME testSaddlePointLinearSolver_2x2_par_8procs_Cartesian_4                 COMMAND "${MPIEXEC}" "-n" "8" "./testSaddlePointLinearSolver_2x2_par"                 "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/2DCartesian/Cartesian_PolyMAC_4_vp.petsc" "-nU" "2112" "-nP" "1024")
 add_test(NAME testSaddlePointLinearSolver_2x2_par_2procs_Cartesian_5                 COMMAND "${MPIEXEC}" "-n" "2" "./testSaddlePointLinearSolver_2x2_par"                 "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/2DCartesian/Cartesian_PolyMAC_5_vp.petsc" "-nU" "8320" "-nP" "4096")
 add_test(NAME testSaddlePointLinearSolver_2x2_par_2procs_Assembly_1                  COMMAND "${MPIEXEC}" "-n" "2" "./testSaddlePointLinearSolver_2x2_par"                 "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/3DAssemblyMesh/Assembly_PolyMAC_1_vp.petsc" "-nU" "20344" "-nP" "9552")
 add_test(NAME testSaddlePointLinearSolver_2x2_par_4procs_Assembly_1                  COMMAND "${MPIEXEC}" "-n" "4" "./testSaddlePointLinearSolver_2x2_par"                 "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/3DAssemblyMesh/Assembly_PolyMAC_1_vp.petsc" "-nU" "20344" "-nP" "9552")
 add_test(NAME testSaddlePointLinearSolver_2x2_par_8procs_Assembly_1                  COMMAND "${MPIEXEC}" "-n" "8" "./testSaddlePointLinearSolver_2x2_par"                 "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/3DAssemblyMesh/Assembly_PolyMAC_1_vp.petsc" "-nU" "20344" "-nP" "9552")
 add_test(NAME testSaddlePointLinearSolver_2x2_par_instrumentation_2procs_Assembly_1  COMMAND "${MPIEXEC}" "-n" "2" "./testSaddlePointLinearSolver_2x2_par_instrumentation" "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/3DAssemblyMesh/Assembly_PolyMAC_1_vp.petsc" "-nU" "20344" "-nP" "9552")
 add_test(NAME testSaddlePointLinearSolver_2x2_par_instrumentation_4procs_Assembly_1  COMMAND "${MPIEXEC}" "-n" "4" "./testSaddlePointLinearSolver_2x2_par_instrumentation" "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/3DAssemblyMesh/Assembly_PolyMAC_1_vp.petsc" "-nU" "20344" "-nP" "9552")
 add_test(NAME testSaddlePointLinearSolver_2x2_par_instrumentation_8procs_Assembly_1  COMMAND "${MPIEXEC}" "-n" "8" "./testSaddlePointLinearSolver_2x2_par_instrumentation" "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/3DAssemblyMesh/Assembly_PolyMAC_1_vp.petsc" "-nU" "20344" "-nP" "9552")
endif( SaddlePointLinearSolver_WITH_MPI )

 install(TARGETS testSaddlePointLinearSolver_initial_seq DESTINATION tests)
 install(TARGETS testSaddlePointLinearSolver_2x2_APIRight DESTINATION tests)
 install(TARGETS testSaddlePointLinearSolver_2x2_par DESTINATION tests)

 add_subdirectory(tests)
endif( SaddlePointLinearSolver_WITH_TESTS )

