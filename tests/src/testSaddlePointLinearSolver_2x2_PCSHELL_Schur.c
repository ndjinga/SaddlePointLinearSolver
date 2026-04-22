static char help[] = "Read a PETSc matrix from a file -f0 <input file>\n Parameters : \n -f0 : matrix fileName \n -nU :number of velocity lines \n -nP : number of pressure lines \n -mat_type : PETSc matrix type \n ";

/*************************************************************************************************/
/* Parallel implementation of a Schur type preconditioner for the linear system A_{input} X_{output} = b_{input} */
/*                                                                                               */
/* Description : Parallel file with PC_FIELDSPLIT_SCHUR_PRE type, restricted to 2x2 blocs.*/
/*               Use of API (class SaddlePointLinearSolve) for better code factorisation         */ 
/*                                                                                               */
/* Input  : - Matrix A_{input}    (system matrix, loaded from a file)                            */
/*          - Vector b_{input}    (right hand side, made up for testing)                         */
/* Output : - Vector X_{output}   (unknown vector, to be determined)                             */
/*                                                                                               */
/*************************************************************************************************/

#include "SaddlePointLinearSolver.h"
#include "PCSHELLSaddlePointLinearSolver.h"

int main( int argc, char **args ){
	PetscInitialize(&argc,&args, (char*)0,help);
	PetscInt n_u, n_p, iter;//Total number of velocity and pressure lines. n = n_u+ n_p
	char file[1][PETSC_MAX_PATH_LEN], mat_type[256]; // File to load, matrix type
	Mat A_input;
	Vec b_input, X_anal, X_output;
	double error,  rtol=1e-7, residu;
    PC pc;
    KSP ksp;
    KSPType ksp_type = KSPFBCGS;//BCGS seems very efficient
    
    /* Minimum profiling for cpu time */
	PetscLogStage  linear_system_stage;
	PetscEventPerfInfo info_linear_system_stage;
	PetscLogDefaultBegin();//This is somehow equivalent to the command line option -log_view but does not display info in the terminal

	PetscBool flg;
	PetscOptionsGetString(NULL,NULL,"-f0",file[0],PETSC_MAX_PATH_LEN,&flg);
	PetscStrcpy(mat_type,MATAIJ);// Default value for PETSc Matrix type
	PetscOptionsGetString(NULL,NULL,"-mat_type",mat_type,sizeof(mat_type),NULL);
	PetscOptionsGetInt(NULL,NULL,"-nU",&n_u,NULL);
	PetscOptionsGetInt(NULL,NULL,"-nP",&n_p,NULL);

	loadPETScMat( file[0], mat_type, &A_input, n_u, n_p);	
	buildRHSVector( A_input, n_u, n_p, &X_anal, &b_input);

//##### Calling KSP solver and monitor convergence
	PetscLogStageRegister("Résolution du système linéaire", &linear_system_stage);//Instrumentation : début de la résolution du second membre
	PetscLogStagePush( linear_system_stage);//Instrumentation

    KSPCreate(PETSC_COMM_WORLD,&ksp);
    KSPSetType(ksp, ksp_type);
    PetscCall( KSPSetOperators(ksp,A_input,A_input) );
    KSPSetTolerances(ksp,rtol, PETSC_DEFAULT,PETSC_DEFAULT,PETSC_DEFAULT);
    KSPGetPC(ksp,&pc);
    PCSetType(pc,PCSHELL);

    SaddlePointCtx2x2 ctx = 
    {
      .n_u = n_u,             /* indices of velocity lines */
      .n_p = n_p              /* indices of pressure lines */
    };
    PCShellSetContext(pc,&ctx);
    PCShellSetApply(pc,applyPC2x2);
    PCShellSetSetUp(pc,setupPC2x2);                   
    PCShellSetDestroy(pc,destroyPC2x2);               
    PetscCall( KSPSetFromOptions(ksp) );
    PetscCall( KSPSetUp(ksp) );

	VecDuplicate(b_input,&X_output);// X_output will store the numerical solution of the linear system
    PetscPrintf(PETSC_COMM_WORLD,"\n Solving the linear system A_input*X_output = b_input with a Schur preconditioner...\n");
    PetscCall( KSPSolve(ksp,b_input, X_output) );

	PetscLogStagePop();//Instrumentation : fin de la résolution du second membre
	PetscLogStageGetPerfInfo( linear_system_stage, &info_linear_system_stage);
	PetscPrintf(PETSC_COMM_WORLD, "\nTime taken to solve the linear system : %e \n\n",info_linear_system_stage.time);
	
//##### Check the solution is correct
    VecAXPY(  X_output, -1, X_anal);
    VecNorm(  X_output, NORM_2, &error);

    KSPGetResidualNorm( ksp, &residu);
    KSPGetIterationNumber(ksp,&iter);
    
    PetscPrintf(PETSC_COMM_WORLD,"Nombre d'itérations = %d\n\n", iter);
    PetscPrintf(PETSC_COMM_WORLD,"L2 Error : ||X_anal - X_num|| = %e\n", error);
    PetscPrintf(PETSC_COMM_WORLD,"L2 residual ||A*X_num - b|| = %e\n\n", residu);

	PetscCheck( error < 1e6*residu, PETSC_COMM_WORLD, PETSC_ERR_NOT_CONVERGED, "Linear system did not return accurate solution. Error is too high compared to residual (e>1e6*r) : e=%e, r=%e\n", error, residu);
	
//##### Cleaning of the memory
    KSPDestroy(&ksp);
	MatDestroy(&A_input);

	VecDestroy(&b_input);
	VecDestroy(&X_anal);
	VecDestroy(&X_output);

	PetscFinalize();
	return 0;
}
