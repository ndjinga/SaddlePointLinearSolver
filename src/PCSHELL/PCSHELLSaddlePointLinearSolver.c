#include "PCSHELLSaddlePointLinearSolver.h"

/* USAGE
  extern PetscErrorCode apply(PC,Vec,Vec);
  extern PetscErrorCode applyba(PC,PCSide,Vec,Vec,Vec);
  extern PetscErrorCode setup(PC);
  extern PetscErrorCode destroy(PC);

  PCCreate(comm,&pc);
  PCSetType(pc,PCSHELL);
  PCShellSetContext(pc,ctx)
  PCShellSetApply(pc,apply);
  PCShellSetApplyBA(pc,applyba);               
  PCShellSetSetUp(pc,setup);                   
  PCShellSetDestroy(pc,destroy);               
*/
