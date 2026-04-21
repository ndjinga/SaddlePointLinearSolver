static char help[] = "Read a PETSc matrix from a file -f0 <input file>\n Parameters : \n -f0 : matrix fileName \n -nU :number of velocity lines \n -nP : number of pressure lines \n -mat_type : PETSc matrix type \n ";

/*************************************************************************************************/
/* Parallel implementation of a transform-then-solve preconditioner for the linear system A_{input} X_{output} = b_{input} */
/*            Use a lower block triangular matrix L and solve the system A_{hat}*X_{output} = b_{hat} with b_hat = L*b_input, A_hat = L*A_{input} */
/*            The transformed matrix Ahat is close to an upper triangular matrix Pmat used as preconditioner */
/*            Due to PETsc block Gauss-Seidel being optimised for lower triangular matrices, Pressure and velocity unknowns are swaped in the transform for better performance */ 
/*                                                                                               */
/* Description : Parallel file with PC_COMPOSITE of MULTIPLICATIVE type, not restricted to 2x2 blocs.*/
/*               Use of API (class SaddlePointLinearSolve) for better code factorisation         */ 
/*                                                                                               */
/* Input  : - Matrix A_{input}    (system matrix, loaded from a file)                            */
/*          - Vector b_{input}    (right hand side, made up for testing)                         */
/* Output : - Vector X_{output}   (unknown vector, to be determined)                             */
/*                                                                                               */
/* Auxilliary variables : - A_hat (transformed matrix)                                           */
/*                        - b_hat (RHS of the transformed system)                            */
/*                        - Pmat  (preconditioning upper triangular matrix)                                       */
/*                        - M top    left  submatrix of A_{input}                                */
/*                        - G top    right submatrix of A_{input}                                */
/*                        - D bottom left  submatrix of A_{input}                                */
/*                        - C bottom right submatrix of A_{input}                                */
/*                                                                                               */
/*                                 *M   G*                                                       */
/*                        A     = *       *                                                      */
/*                                 *D   C*                                                       */
/*                                                                                               */
/*                                 *Id               0*                    *Id              0*   */
/*                        L     = *                    *         L^{-1} = *                   *  */
/*                                 *-D*diag(M)^{-1} Id*                    *D*diag(M)^{-1} Id*   */
/*                                                                                               */
/*                        WITHOUT SWAPP of velocity and pressure                                                                       */
/*                                                                                               */
/*                                 *M           G*             D_hat=D - D*diag(M)^{-1}*M          */
/*                        A_hat = *               *                                                */
/*                                 *D_hat   C_hat*             C_hat=C - D*diag(M)^{-1}*G          */
/*                                                                                               */
/*                                 *2 diag(M)     G  *                                           */
/*                        Pmat  = *                   *                                          */
/*                                 *0          C_hat *                                           */
/*                                                                                               */
/*                        WITH SWAPP of velocity and pressure                                                                       */
/*                                                                                               */
/*                                 *C_hat   D_hat*             D_hat=D - D*diag(M)^{-1}*M          */
/*                        A_hat = *               *                                                */
/*                                 *G           M*             C_hat=C - D*diag(M)^{-1}*G          */
/*                                                                                               */
/*                                 *C_hat     0  *                                           */
/*                        Pmat  = *               *                                          */
/*                                 *G   2 diag(M)*                                           */
/*                                                                                               */
/*************************************************************************************************/

#include "SaddlePointLinearSolver.h"

int main( int argc, char **args ){
	PetscInitialize(&argc,&args, (char*)0,help);
	PetscInt n_u, n_p, n;//Total number of velocity and pressure lines. n = n_u+ n_p
	char file[1][PETSC_MAX_PATH_LEN], mat_type[256]; // File to load, matrix type
	Mat A_input, A_hat, Pmat;
	Mat M, G, D, C;
	IS is_U,is_P;
	Vec b_input, b_hat, X_output, X_anal;
	Vec v;
	double error,  rtol=1e-7, residu;
    PetscBool useLowerTriangularTransform = PETSC_TRUE;
    
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
	n=n_u+n_p;
    
	loadPETScMat( file[0], mat_type, &A_input, n_u, n_p);
	
	splitPETScMatrix2x2(   A_input, n_u, n_p, &M, &G, &D, &C, &is_U, &is_P);

	buildRHSVector( A_input, n_u, n_p, &X_anal, &b_input);

	PetscLogStageRegister("Résolution du système linéaire", &linear_system_stage);//Instrumentation : début de la résolution du second membre
	PetscLogStagePush( linear_system_stage);//Instrumentation
	transformSystemLeft(M,G,D,C,&A_hat,&Pmat, &v, useLowerTriangularTransform);

    getbhatFrombinput( D, v, b_input, &b_hat, is_U, is_P, useLowerTriangularTransform);

//##### Calling KSP solver and monitor convergence
	VecDuplicate( X_anal, &X_output);

    /* In order to work with lower triangular matrices we need to swap u and p in the unknown vector */
    /* Change the index set because of the permutation u<->p in the unknown vector */
    if( useLowerTriangularTransform )
    {
	  ISShift( is_U,  n_p, is_U);
	  ISShift( is_P, -n_u, is_P);        
    }

    solveLeftTransformedSystemForXoutput( A_hat, Pmat, is_U, is_P, b_hat, &X_output, rtol,PETSC_DEFAULT,PETSC_DEFAULT, PETSC_DEFAULT, &residu, useLowerTriangularTransform);
	PetscLogStagePop();//Instrumentation : fin de la résolution du second membre
	PetscLogStageGetPerfInfo( linear_system_stage, &info_linear_system_stage);
	PetscPrintf(PETSC_COMM_WORLD, "\nTime taken to solve the linear system : %e \n\n",info_linear_system_stage.time);

	Vec X_p;//Pressure components of the main unknown
	Vec X_u;//Velocity components of the transformed unknown
	PetscCall( VecGetSubVector( X_output, is_P, &X_p) );
	PetscCall( VecGetSubVector( X_output, is_U, &X_u) );

    /* Change the index set because of the permutation u<->p in the unknown vector */
    if( useLowerTriangularTransform )
    {
	  ISShift( is_U, -n_p, is_U);
	  ISShift( is_P,  n_u, is_P);        
    }

	error = computeErrorAndCheck( X_anal, X_output, is_U, is_P, X_u, X_p);	
	PetscCheck( error < 1e6*residu, PETSC_COMM_WORLD, PETSC_ERR_NOT_CONVERGED, "Linear system did not return accurate solution. Error is too high compared to residual (e>1e6*r) : e=%e, r=%e\n", error, residu);
	
//##### Cleaning of the memory
	MatDestroy(&A_input);
	MatDestroy(&A_hat);
	MatDestroy(&Pmat);
	MatDestroy(&M);
	MatDestroy(&D);	
	MatDestroy(&G);
	MatDestroy(&C);

	VecDestroy(&b_input);
	VecDestroy(&b_hat);
	VecDestroy(&X_anal);
	VecDestroy(&v);
	VecDestroy(&X_u);
	VecDestroy(&X_p);
	VecDestroy(&X_output);

	ISDestroy(&is_U);
	ISDestroy(&is_P);

	PetscFinalize();
	return 0;
}
