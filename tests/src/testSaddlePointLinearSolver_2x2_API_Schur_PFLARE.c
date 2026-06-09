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

int main( int argc, char **args ){
	PetscInitialize(&argc,&args, (char*)0,help);
	PetscInt n_u, n_p;//Total number of velocity and pressure lines. n = n_u+ n_p
	char file[1][PETSC_MAX_PATH_LEN], mat_type[256]; // File to load, matrix type
	Mat A_input;
	Mat M, G, D, C;
	IS is_U,is_P;
	Vec b_input, X_anal, X_output, X_u, X_p;
	double error,  rtol=1e-7, residu;

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
	splitPETScMatrix2x2(   A_input, n_u, n_p, &M, &G, &D, &C, &is_U, &is_P);//Only the index sets is_U and is_P will be used, the splitting of the matrix is not necessary
	buildRHSVector( A_input, n_u, n_p, &X_anal, &b_input);

//##### Calling KSP solver and monitor convergence
	PetscLogStageRegister("Résolution du système linéaire", &linear_system_stage);//Instrumentation : début de la résolution du second membre
	PetscLogStagePush( linear_system_stage);//Instrumentation

	VecDuplicate(b_input,&X_output);// X_output will store the numerical solution of the linear system
	solveSchurPFLARESystemForXoutput( A_input, M, G, D, C, is_U, is_P, b_input, &X_output, rtol,PETSC_DEFAULT,PETSC_DEFAULT, PETSC_DEFAULT, &residu);

	PetscLogStagePop();//Instrumentation : fin de la résolution du second membre
	PetscLogStageGetPerfInfo( linear_system_stage, &info_linear_system_stage);
	PetscPrintf(PETSC_COMM_WORLD, "\nTime taken to solve the linear system : %e \n\n",info_linear_system_stage.time);
	
//##### Check the solution is correct
	PetscCall( VecGetSubVector( X_output, is_P, &X_p) );
	PetscCall( VecGetSubVector( X_output, is_U, &X_u) );

	error = computeErrorAndCheck( X_anal, X_output, is_U, is_P, X_u, X_p);	
	PetscCheck( error < 1e6*residu, PETSC_COMM_WORLD, PETSC_ERR_NOT_CONVERGED, "Linear system did not return accurate solution. Error is too high compared to residual (e>1e6*r) : e=%e, r=%e\n", error, residu);
	
//##### Cleaning of the memory
	MatDestroy(&A_input);
	MatDestroy(&M);
	MatDestroy(&D);	
	MatDestroy(&G);
	MatDestroy(&C);

	VecDestroy(&b_input);
	VecDestroy(&X_anal);
	VecDestroy(&X_u);
	VecDestroy(&X_p);
	VecDestroy(&X_output);

	ISDestroy(&is_U);
	ISDestroy(&is_P);

	PetscFinalize();
	return 0;
}
