cmake_minimum_required(VERSION 3.12.0)

set(MY_SOURCES
SaddlePointLinearSolver.c
PCSHELL/PCSHELLSaddlePointLinearSolver.c
)

add_library(SaddlePointLinearSolverLib SHARED ${MY_SOURCES})
target_include_directories(SaddlePointLinearSolverLib PRIVATE ${PETSC_INCLUDES} ${MY_HEADERS})
install(TARGETS SaddlePointLinearSolverLib DESTINATION lib)


