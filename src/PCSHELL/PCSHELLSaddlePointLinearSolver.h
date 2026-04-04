#include <petscis.h>
#include <petscksp.h>

/*
     User-defined application context
*/
typedef struct {
   Vec         v;     /* Diagonal of the bloc M */
   PetscMPIInt rank;  /* rank of processor */
   PetscMPIInt size;  /* size of communicator */
   PetscMPIInt n_U;  /* number of velocity lines */
   PetscMPIInt n_P;  /* number of pressure lines */
 } ApplicationCtx;


extern PetscErrorCode apply(PC,Vec,Vec);
extern PetscErrorCode applyba(PC,PCSide,Vec,Vec,Vec);
extern PetscErrorCode setup(PC);
extern PetscErrorCode destroy(PC);
