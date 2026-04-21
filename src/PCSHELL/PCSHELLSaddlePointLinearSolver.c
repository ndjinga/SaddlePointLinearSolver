#include "PCSHELLSaddlePointLinearSolver.h"

/* USAGE

  PCCreate(comm,&pc);
  PCSetType(pc,PCSHELL);
  PCShellSetContext(pc,ctx)
  PCShellSetApply(pc,apply);
  PCShellSetSetUp(pc,setup);                   
  PCShellSetDestroy(pc,destroy);               
*/

/* setup function for the preconditioner */
PetscErrorCode setup(PC pcshell)
{
    SaddlePointCtx2x2 * ctx;
    PC               subpc1;     /* The PCFIELDSPLIT sub-preconditioner1 */
    PC               subpc2;     /* The PCFIELDSPLIT sub-preconditioner2 */
    PetscInt nrows, ncolumns;//Total number of rows and columns of A_input
    PetscInt irow_min, irow_max;//min and max indices of rows stored locally on this process
	PetscInt n_u, n_p;//Total number of velocity and pressure lines. n = n_u+ n_p


    PetscFunctionBegin;

//#### Building the index sets is_U and is_P
    PetscCall(PCShellGetContext( pcshell, &ctx));
    n_u = ctx->n_u;
    MatGetOwnershipRange( ctx->A_input, &irow_min, &irow_max);
    MatGetSize( ctx->A_input, &nrows, &ncolumns);
    PetscInt min_pressure_lines = irow_min <= n_u ? n_u : irow_min;//max(irow_min, n_u)
    PetscInt max_velocity_lines = irow_max >= n_u ? n_u : irow_max;//min(irow_max, n_u)
    PetscInt nb_pressure_lines = irow_max >= n_u ? irow_max - min_pressure_lines : 0;
    PetscInt nb_velocity_lines = irow_min <= n_u ? max_velocity_lines - irow_min : 0;
    PetscInt nb_local_lines = irow_max - irow_min; 

    PetscCheck(   nrows == ncolumns, PETSC_COMM_WORLD, PETSC_ERR_ARG_SIZ, "Matrix is not square !!!\n");
    PetscCheck( n_u+n_p == ncolumns, PETSC_COMM_WORLD, PETSC_ERR_ARG_SIZ, "Inconsistent data : the matrix has %d lines but only %d velocity lines and %d pressure lines declared\n", ncolumns, n_u,n_p);
    
    ISCreateStride(PETSC_COMM_WORLD, nb_velocity_lines, max_velocity_lines - nb_velocity_lines, 1, is_U);
    ISCreateStride(PETSC_COMM_WORLD, nb_pressure_lines, min_pressure_lines                    , 1, is_P);

    PetscFunctionReturn(PETSC_SUCCESS);
}

/* destroy function for the preconditioner */
PetscErrorCode destroy(PC pcshell)
{
    SaddlePointCtx2x2 * ctx;
    
    PetscFunctionBegin;
    PetscCall(PCShellGetContext( pcshell, &ctx));
    VecDestroy(&ctx->GX_p);
    VecDestroy(&ctx->Y_u);
    VecDestroy(&ctx->Y_p);
    VecDestroy(&ctx->X_u);
    VecDestroy(&ctx->X_p);
    PCDestroy(&ctx->pcM);
    PetscFunctionReturn(PETSC_SUCCESS);
}

/* multiplication operator for the preconditioner */
PetscErrorCode apply(PC pcshell, Vec x, Vec y)
{
    SaddlePointCtx2x2 * ctx;
    
    PetscFunctionBegin;
    PetscCall(PCShellGetContext( pcshell, &ctx));
    PetscCall(VecGetSubVector( x, ctx->is_P, &ctx->X_p) );
    PetscCall(VecGetSubVector( x, ctx->is_U, &ctx->X_u) );
    PetscCall(VecGetSubVector( y, ctx->is_P, &ctx->Y_p) );
    PetscCall(VecGetSubVector( y, ctx->is_U, &ctx->Y_u) );

    PetscCall(MatMult( ctx->G,   ctx->X_p,  ctx->GX_p) );//GX_p contains G*x_p
    PetscCall(PCApply( ctx->pcM, ctx->GX_p, ctx->Y_u ) );//Y_u contains M^{-1}*G*x_p
    PetscCall(VecAYPX( ctx->Y_u, -1, ctx->X_u) );//X_u contains X_u - M^{-1}*G*x_p
    VecCopy(ctx->X_p,ctx->Y_p);

    PetscCall(VecRestoreSubVector( x, ctx->is_P, &ctx->X_p) );
    PetscCall(VecRestoreSubVector( x, ctx->is_U, &ctx->X_u) );
    PetscFunctionReturn(PETSC_SUCCESS);
}

