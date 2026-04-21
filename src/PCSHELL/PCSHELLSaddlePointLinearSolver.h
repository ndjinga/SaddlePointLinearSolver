#include <petscis.h>
#include <petscksp.h>

/*
     User-defined preconditioner context
*/
typedef struct {
   PC           pc;       /* The PCFIELDSPLIT main preconditioner (output)*/
   Mat     A_input;       /* The system matrix */
   PetscMPIInt n_U;       /* number of velocity lines (input)*/
   PetscMPIInt n_P;       /* number of pressure lines (input)*/
 } SaddlePointCtx2x2;


PetscErrorCode apply(PC,Vec,Vec);
PetscErrorCode setup(PC);
PetscErrorCode destroy(PC);
