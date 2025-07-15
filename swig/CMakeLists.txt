INCLUDE(${SWIG_USE_FILE})

SET_SOURCE_FILES_PROPERTIES(SaddlePointLinearSolver.i PROPERTIES CPLUSPLUS OFF)
SET_SOURCE_FILES_PROPERTIES(SaddlePointLinearSolver.i PROPERTIES C ON)
SET_SOURCE_FILES_PROPERTIES(SaddlePointLinearSolver.i PROPERTIES SWIG_DEFINITIONS "-shadow")

INCLUDE_DIRECTORIES(
  ${Python_INCLUDE_DIRS}
  ${PETSC_INCLUDES}	
  ${MY_HEADERS}								    #
)

if(     EXISTS ${PETSC4PY_ROOT_DIR}/petsc4py/include/petsc4py/petsc4py.i)
  INCLUDE_DIRECTORIES( ${PETSC4PY_ROOT_DIR}/petsc4py/include
                       ${PETSC4PY_ROOT_DIR}/petsc4py/include/petsc4py )
elseif (EXISTS ${PETSC4PY_ROOT_DIR}/include/petsc4py/petsc4py.i )
  INCLUDE_DIRECTORIES( ${PETSC4PY_ROOT_DIR}/include
                       ${PETSC4PY_ROOT_DIR}/include/petsc4py )
endif(  EXISTS ${PETSC4PY_ROOT_DIR}/petsc4py/include/petsc4py/petsc4py.i)

IF   (${CMAKE_VERSION} VERSION_LESS "3.8.0") 
  SWIG_ADD_MODULE(SaddlePointLinearSolver python SaddlePointLinearSolver.i)
ELSE (${CMAKE_VERSION} VERSION_LESS "3.8.0") 
  SWIG_ADD_LIBRARY(SaddlePointLinearSolver TYPE SHARED LANGUAGE python SOURCES SaddlePointLinearSolver.i)
ENDIF(${CMAKE_VERSION} VERSION_LESS "3.8.0")

SWIG_LINK_LIBRARIES(SaddlePointLinearSolver ${Python_LIBRARIES} SaddlePointLinearSolverLib ${PETSC_LIBRARIES} )

INSTALL(TARGETS _SaddlePointLinearSolver DESTINATION lib/SaddlePointLinearSolver)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/SaddlePointLinearSolver.py DESTINATION bin/SaddlePointLinearSolver)


