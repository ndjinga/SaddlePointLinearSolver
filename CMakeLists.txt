cmake_minimum_required(VERSION 3.12.0)

project("SaddlePointLinearSolver")
set(PROJECT_BRIEF "A preconditioner for linear systems with 2x2 block matrices")

list (APPEND CMAKE_MODULE_PATH "${SaddlePointLinearSolver_SOURCE_DIR}/cmake")

option (SaddlePointLinearSolver_WITH_PYTHON     "Build Python interface of freesteam" ON)
option (SaddlePointLinearSolver_WITH_TESTS      "Build and run FREESTEAM example tests" ON)
option (SaddlePointLinearSolver_WITH_DOCUMENTATION "Generate SOLVERLAB documentation" ON)
option (SaddlePointLinearSolver_WITH_MPI           "Generate a parallel version of SOLVERLAB" OFF)

#Path to installed libraries
set (PYTHON_ROOT_DIR      OFF CACHE STRING "Python library path" )
set (PETSC_DIR            OFF CACHE STRING "PETSc library path" )
set (PETSC_ARCH           OFF CACHE STRING "PETSc architecture" )
set (MPI_HOME             OFF CACHE STRING "MPI library path" )

set(MY_SOURCES
src/SaddlePointLinearSolver.c
)
set(MY_HEADERS
src/
)

add_library(SaddlePointLinearSolverLib SHARED ${MY_SOURCES})
message(STATUS ${PETSC_INCLUDES})
target_include_directories(SaddlePointLinearSolverLib PRIVATE ${PETSC_INCLUDES} ${MY_HEADERS})
install(TARGETS SaddlePointLinearSolverLib DESTINATION lib)

find_package (PETSc 3.4 REQUIRED)

if( SaddlePointLinearSolver_WITH_PYTHON )
  message(STATUS "SaddlePointLinearSolverm requested with PYTHON library. Searching PYTHON library.")
  find_package(Python ${PYTHON2OR3} REQUIRED COMPONENTS Interpreter Development )
  SET(PYTHON_EXECUTABLE ${Python_EXECUTABLE})
endif( SaddlePointLinearSolver_WITH_PYTHON )

if( SaddlePointLinearSolver_WITH_MPI )
  message(STATUS "SaddlePointLinearSolverm requested with MPI library. Searching MPI library.")
  if   ( NOT MPI_HOME)
    set( MPI_HOME ${PETSC_DIR}/${PETSC_ARCH} )
  endif( NOT MPI_HOME)
  find_package(MPI REQUIRED)
endif( SaddlePointLinearSolver_WITH_MPI )

if( SaddlePointLinearSolver_WITH_TESTS )
 enable_testing ()

 set(MY_TEST_SOURCES
 tests/src/testSaddlePointLinearSolver_initial.c
 tests/src/testSaddlePointLinearSolver_2x2_seq.c
 tests/src/testSaddlePointLinearSolver_2x2_par.c
 tests/src/testSaddlePointLinearSolver_2x2_par_instrumentation.c
 #tests/src/testSaddlePointLinearSolver_nxn_par.c
 #tests/src/testSaddlePointLinearSolver_2x2_Shat.c
 )

 # build test main exe
 add_executable(testSaddlePointLinearSolver_initial                 tests/src/testSaddlePointLinearSolver_initial.c)
 add_executable(testSaddlePointLinearSolver_2x2_seq                 tests/src/testSaddlePointLinearSolver_2x2_seq.c)
 add_executable(testSaddlePointLinearSolver_2x2_par                 tests/src/testSaddlePointLinearSolver_2x2_par.c)
 add_executable(testSaddlePointLinearSolver_2x2_par_instrumentation tests/src/testSaddlePointLinearSolver_2x2_par_instrumentation.c)
 target_include_directories(testSaddlePointLinearSolver_initial                 PRIVATE ${PETSC_INCLUDES} )
 target_include_directories(testSaddlePointLinearSolver_2x2_seq                 PRIVATE ${PETSC_INCLUDES}  ${MY_HEADERS})
 target_include_directories(testSaddlePointLinearSolver_2x2_par                 PRIVATE ${PETSC_INCLUDES} )
 target_include_directories(testSaddlePointLinearSolver_2x2_par_instrumentation PRIVATE ${PETSC_INCLUDES} )
 target_link_libraries(testSaddlePointLinearSolver_2x2_seq SaddlePointLinearSolverLib )

 target_link_libraries(testSaddlePointLinearSolver_initial                 ${PETSC_LIBRARIES} ${MPI_HOME}/lib/libmpi.so)
 target_link_libraries(testSaddlePointLinearSolver_2x2_seq                 ${PETSC_LIBRARIES} ${MPI_HOME}/lib/libmpi.so)
 target_link_libraries(testSaddlePointLinearSolver_2x2_par                 ${PETSC_LIBRARIES} ${MPI_HOME}/lib/libmpi.so)
 target_link_libraries(testSaddlePointLinearSolver_2x2_par_instrumentation ${PETSC_LIBRARIES} ${MPI_HOME}/lib/libmpi.so)

 add_test(NAME testSaddlePointLinearSolver_initial_Assembly_1          COMMAND "./testSaddlePointLinearSolver_initial"         "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/AssemblyMesh/Assembly_PolyMAC_1_vp.petsc" "-nU" "20344" "-nP" "9552" "-pc_fieldsplit_type" "schur" "-pc_fieldsplit_schur_fact_type" "lower" "-fieldsplit_0_ksp_type" "gmres" "-fieldsplit_0_pc_type" "gamg" "-fieldsplit_0_pc_gamg_type" "agg" "-fieldsplit_1_ksp_type" "preonly" "-fieldsplit_1_pc_type" "lu")
 add_test(NAME testSaddlePointLinearSolver_2x2_seq_Assembly_1  COMMAND "./testSaddlePointLinearSolver_2x2_seq" "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/AssemblyMesh/Assembly_PolyMAC_1_vp.petsc" "-nU" "20344" "-nP" "9552" "-pc_fieldsplit_type" "schur" "-pc_fieldsplit_schur_fact_type" "lower" "-fieldsplit_0_ksp_type" "gmres" "-fieldsplit_0_pc_type" "gamg" "-fieldsplit_0_pc_gamg_type" "agg" "-fieldsplit_1_ksp_type" "preonly" "-fieldsplit_1_pc_type" "lu")
 add_test(NAME testSaddlePointLinearSolver_2x2_par_Assembly_1  COMMAND "./testSaddlePointLinearSolver_2x2_par" "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/AssemblyMesh/Assembly_PolyMAC_1_vp.petsc" "-nU" "20344" "-nP" "9552")
 add_test(NAME testSaddlePointLinearSolver_2x2_par_instrumentation_Assembly_1  COMMAND "./testSaddlePointLinearSolver_2x2_par_instrumentation" "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/AssemblyMesh/Assembly_PolyMAC_1_vp.petsc" "-nU" "20344" "-nP" "9552")
 add_test(NAME testSaddlePointLinearSolver_2x2_par_instrumentation_Assembly_5  COMMAND "./testSaddlePointLinearSolver_2x2_par_instrumentation" "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/AssemblyMesh/Assembly_PolyMAC_5_vp.petsc" "-nU" "303660" "-nP" "131979")

if( SaddlePointLinearSolver_WITH_MPI )
 add_test(NAME testSaddlePointLinearSolver_2x2_par_2procs_Assembly_1                  COMMAND "${MPIEXEC}" "-n" "2" "./testSaddlePointLinearSolver_2x2_par" "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/AssemblyMesh/Assembly_PolyMAC_1_vp.petsc" "-nU" "20344" "-nP" "9552")
 add_test(NAME testSaddlePointLinearSolver_2x2_par_instrumentation_2procs_Assembly_1  COMMAND "${MPIEXEC}" "-n" "2" "./testSaddlePointLinearSolver_2x2_par_instrumentation" "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/AssemblyMesh/Assembly_PolyMAC_1_vp.petsc" "-nU" "20344" "-nP" "9552")
 add_test(NAME testSaddlePointLinearSolver_2x2_par_instrumentation_2procs_Assembly_5  COMMAND "${MPIEXEC}" "-n" "2" "./testSaddlePointLinearSolver_2x2_par_instrumentation" "-f0" "${PROJECT_SOURCE_DIR}/tests/data/matrices/AssemblyMesh/Assembly_PolyMAC_5_vp.petsc" "-nU" "303660" "-nP" "131979")
endif( SaddlePointLinearSolver_WITH_MPI )

 install(TARGETS testSaddlePointLinearSolver_initial DESTINATION tests)
 install(TARGETS testSaddlePointLinearSolver_2x2_seq DESTINATION tests)
 install(TARGETS testSaddlePointLinearSolver_2x2_par DESTINATION tests)

 add_subdirectory(tests)
endif( SaddlePointLinearSolver_WITH_TESTS )

