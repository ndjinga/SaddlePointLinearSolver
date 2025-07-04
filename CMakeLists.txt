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

set(MY_SOURCES
#SaddlePointLinearSolver.c
#SaddlePointLinearSolver_par.c
#SaddlePointLinearSolver_par2.c
#SaddlePointLinearSolver_Shat.c
)

#add_library(SaddlePointLinearSolverLib SHARED ${MY_SOURCES})
#install(TARGETS SaddlePointLinearSolverLib DESTINATION lib)

find_package (PETSc 3.4 REQUIRED)

if( SaddlePointLinearSolver_WITH_PYTHON )
  message(STATUS "SaddlePointLinearSolverm requested with PYTHON library. Searching PYTHON library.")
  find_package(Python ${PYTHON2OR3} REQUIRED COMPONENTS Interpreter Development )
  SET(PYTHON_EXECUTABLE ${Python_EXECUTABLE})
endif( SaddlePointLinearSolver_WITH_PYTHON )


if( SaddlePointLinearSolver_WITH_TESTS )
 enable_testing ()

 set(MY_TEST_SOURCES
 #SaddlePointLinearSolver.c
 tests/src/SaddlePointLinearSolver_par.c
 #SaddlePointLinearSolver_par2.c
 #SaddlePointLinearSolver_Shat.c
 )

 # build test main exe
 add_executable(SaddlePointLinearSolver_test ${MY_TEST_SOURCES})
 target_include_directories(SaddlePointLinearSolver_test PRIVATE ${PETSC_INCLUDES} )
 #target_link_libraries(SaddlePointLinearSolver_test SaddlePointLinearSolverLib )
 target_link_libraries(SaddlePointLinearSolver_test ${PETSC_LIBRARIES})

 add_test(NAME SaddlePointLinearSolver_test  COMMAND "./SaddlePointLinearSolver_test -f0 Matrices/Assembly_PolyMAC_1_vp.petsc -nU 20344 -nP 9552")  
 install(TARGETS SaddlePointLinearSolver_test DESTINATION test)
endif( SaddlePointLinearSolver_WITH_TESTS )


