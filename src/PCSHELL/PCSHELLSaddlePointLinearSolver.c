#include "PCSHELLSaddlePointLinearSolver.h"

/* setup function for the preconditioner */
PetscErrorCode setupPC2x2(PC pcshell)
{
    SaddlePointCtx2x2 * ctx;
    Mat          Amat, Pmat;     /* The system matrix */
    PC               subpc1;     /* The PCFIELDSPLIT sub-preconditioner1 */
    PC               subpc2;     /* The PCFIELDSPLIT sub-preconditioner2 */
    PetscInt nrows, ncolumns;//Total number of rows and columns of A_input
    PetscInt irow_min, irow_max;//min and max indices of rows stored locally on this process
	PetscInt n_u, n_p;//Total number of velocity and pressure lines. n = n_u+ n_p
    KSP *kspArray;
    IS is_U, is_P;

    PetscFunctionBegin;

//#### Building the index sets is_U and is_P
    PetscCall(PCShellGetContext( pcshell, &ctx));
    n_u = ctx->n_u;
    n_p = ctx->n_p;
    PCGetOperators( pcshell, &Amat, &Pmat);
    MatGetOwnershipRange( Pmat, &irow_min, &irow_max);
    MatGetSize( Pmat, &nrows, &ncolumns);
    
    PetscInt min_pressure_lines = irow_min <= n_u ? n_u : irow_min;//max(irow_min, n_u)
    PetscInt max_velocity_lines = irow_max >= n_u ? n_u : irow_max;//min(irow_max, n_u)
    PetscInt nb_pressure_lines = irow_max >= n_u ? irow_max - min_pressure_lines : 0;
    PetscInt nb_velocity_lines = irow_min <= n_u ? max_velocity_lines - irow_min : 0;
    PetscInt nb_local_lines = irow_max - irow_min; 

    PetscCheck(   nrows == ncolumns, PETSC_COMM_WORLD, PETSC_ERR_ARG_SIZ, "Matrix is not square !!!\n");
    PetscCheck( n_u+n_p == ncolumns, PETSC_COMM_WORLD, PETSC_ERR_ARG_SIZ, "Inconsistent data : the matrix has %d lines but only %d velocity lines and %d pressure lines declared.\n Only velocity and pressure unknowns handled now\n", ncolumns, n_u,n_p);
    
    ISCreateStride(PETSC_COMM_WORLD, nb_velocity_lines, max_velocity_lines - nb_velocity_lines, 1, &is_U);
    ISCreateStride(PETSC_COMM_WORLD, nb_pressure_lines, min_pressure_lines                    , 1, &is_P);

//#### The PCFIELDSPLIT preconditioner (based on GAMG and ILU) ###//
    PCCreate(PETSC_COMM_WORLD, &ctx->pc);
    PetscCall( PCSetOperators(ctx->pc,Amat,Pmat) );
    PCSetType( ctx->pc, PCFIELDSPLIT);
    PCFieldSplitSetType( ctx->pc, PC_COMPOSITE_SCHUR);

    PCFieldSplitSetIS( ctx->pc, "velocity",is_U);//The order here matters a lot between this line and the next
    PCFieldSplitSetIS( ctx->pc, "pressure",is_P);//The order here matters a lot between this line and the previous
    PCFieldSplitSetSchurPre(      ctx->pc, PC_FIELDSPLIT_SCHUR_PRE_SELFP,NULL);//or PC_FIELDSPLIT_SCHUR_PRE_USER if you provide Chat
    PCFieldSplitSetSchurFactType( ctx->pc, PC_FIELDSPLIT_SCHUR_FACT_FULL);
    PetscCall( PCSetUp( ctx->pc) );
    PCFieldSplitSchurGetSubKSP( ctx->pc, NULL, &kspArray);
    KSPSetType( kspArray[0], KSPPREONLY);
    KSPSetType( kspArray[1], KSPPREONLY);
    KSPGetPC(kspArray[0], &subpc1);
    KSPGetPC(kspArray[1], &subpc2);

    PCSetType( subpc1, PCJACOBI);
    PCSetType( subpc2, PCBJACOBI);//or PCHYPRE with multigrid

    ISDestroy(&is_U);
    ISDestroy(&is_P);
    PetscFree(kspArray);
    
    PetscFunctionReturn(PETSC_SUCCESS);
}

/* destroy function for the preconditioner */
PetscErrorCode destroyPC2x2(PC pcshell)
{
    SaddlePointCtx2x2 * ctx;
    
    PetscFunctionBegin;
    PetscCall(PCShellGetContext( pcshell, &ctx));
    PCDestroy(&ctx->pc);
    PetscFunctionReturn(PETSC_SUCCESS);
}

/* multiplication operator for the preconditioner */
PetscErrorCode applyPC2x2(PC pcshell, Vec x, Vec y)
{
    SaddlePointCtx2x2 * ctx;
    
    PetscFunctionBegin;
    PetscCall(PCShellGetContext( pcshell, &ctx));

    PetscCall(PCApply( ctx->pc, x, y ) );

    PetscFunctionReturn(PETSC_SUCCESS);
}
