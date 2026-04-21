#include <petscis.h>
#include <petscksp.h>

/*
     User-defined preconditioner context
     For the moment only velocity and pressure unknowns handled (2x2 system)
     - the velocity indices are supposed to be contiguous
     - the pressure indices are supposed to be contiguous
     - the velocity indices come before the pressure indices in the unknown vector
*/
typedef struct {
   PetscInt n_u;       /* number of velocity lines (input) */
   PetscInt n_p;       /* number of pressure lines (input) */
   PC        pc;       /* The PCFIELDSPLIT preconditioner suited for saddle point matrices (output) */
 } SaddlePointCtx2x2;


PetscErrorCode apply(PC,Vec,Vec);
PetscErrorCode setup(PC);
PetscErrorCode destroy(PC);
