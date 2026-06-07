cmake_minimum_required(VERSION 3.12.0)

set(MY_SOURCES
SaddlePointLinearSolver.c
PCSHELL/PCSHELLSaddlePointLinearSolver.c
)

add_library(SaddlePointLinearSolverLib SHARED ${MY_SOURCES})
target_include_directories(SaddlePointLinearSolverLib PRIVATE ${PETSC_INCLUDES} ${MY_HEADERS})
target_link_libraries(SaddlePointLinearSolverLib ${PETSC_LIBRARIES} ${MPI_HOME}/lib/libmpi.so ${HYPRE_LIBRARIES} ${PFLARE_LIB} )
install(TARGETS SaddlePointLinearSolverLib DESTINATION lib)


