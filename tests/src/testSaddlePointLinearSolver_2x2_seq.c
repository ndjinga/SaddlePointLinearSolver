static char help[] = "Read a PETSc matrix from a file -f0 <input file>\n Parameters : \n -f0 : matrix fileName \n -nU :number of velocity lines \n -nP : number of pressure lines \n -mat_type : PETSc matrix type \n";

/*************************************************************************************************/
/* Sequential implementation of a new preconditioner for the linear system A_{input} X_{output} = b_{input} */
/*                                                                                               */
/* Input  : - Matrix A_{input}    (system matrix, loaded from a file)                            */
/*          - Vector b_{input}    (right hand side, made up for testing)                         */
/* Output : - Vector X_{output}   (unknown vector, to be determined                              */
/*                                                                                               */
/* Auxilliary variables : - A_hat (transformed matrix)                                           */
/*                        - X_hat (unknown of the transformed system)                            */
/*                        - b_hat (RHS of the transformed system)                                */
/*                        - Pmat  (preconditioning matrix)                                       */
/*                        - M top    left  submatrix of A_{input}                                */
/*                        - G top    right submatrix of A_{input}                                */
/*                        - D bottom left  submatrix of A_{input}                                */
/*                        - C bottom right submatrix of A_{input}                                */
/*                                                                                               */
/*                                 *M   G*                                                       */
/*                        A     = *       *                                                      */
/*                                 *D   C*                                                       */
/*                                                                                               */
/*                                 *C_hat  -D*                                                   */
/*                        A_hat = *           *                                                  */
/*                                 *G_hat   M*                                                   */
/*                                                                                               */
/*                                 *C_hat        0   *                                           */
/*                        Pmat  = *                   *                                          */
/*                                 *G_hat   2 diag(M)*                                           */
/*                                                                                               */
/*************************************************************************************************/

#include "SaddlePointLinearSolver.h"

int main( int argc, char **args ){
	PetscInitialize(&argc,&args, (char*)0,help);
	PetscMPIInt    size;        /* size of communicator */
	PetscMPIInt    rank;        /* processor rank */
	MPI_Comm_rank(PETSC_COMM_WORLD,&rank);
	MPI_Comm_size(PETSC_COMM_WORLD,&size);
	PetscErrorCode ierr=0;
	char file[1][PETSC_MAX_PATH_LEN], mat_type[256]; // File to load, matrix type
	Mat A_input, A_hat, Pmat;
	Mat M, G, D, C;
	Mat C_hat, G_hat, diag_2M;
	IS is_U,is_P;
	Vec b_input, b_input_p, b_input_u, b_hat, X_hat, X_anal;
	Vec v;
	double error;
	
	PetscCheck( size == 1, PETSC_COMM_WORLD, ierr, "Incorrect number of procs nprocs = %d.\n !!! This is a sequential implementation !!! \n", size);

	PetscBool flg;
	PetscOptionsGetString(NULL,NULL,"-f0",file[0],PETSC_MAX_PATH_LEN,&flg);
	PetscStrcpy(mat_type,MATAIJ);// Default value for PETSc Matrix type
	PetscOptionsGetString(NULL,NULL,"-mat_type",mat_type,sizeof(mat_type),NULL);

	loadPETScMat( file[0], mat_type, &A_input, size);
	
	PetscInt nrows, ncolumns;//Total number of rows and columns of A_input
	PetscInt n_u, n_p, n;//Total number of velocity and pressure lines. n = n_u+ n_p

	PetscOptionsGetInt(NULL,NULL,"-nU",&n_u,NULL);
	PetscOptionsGetInt(NULL,NULL,"-nP",&n_p,NULL);
	n=n_u+n_p;
	MatGetSize( A_input, &nrows, &ncolumns);

	PetscCheck( nrows == ncolumns, PETSC_COMM_WORLD, ierr, "Matrix is not square !!!\n");
	PetscCheck( n == ncolumns, PETSC_COMM_WORLD, ierr, "Inconsistent data : the matrix has %d lines but only %d velocity lines and %d pressure lines declared\n", ncolumns, n_u,n_p);
	PetscPrintf(PETSC_COMM_WORLD,"The matrix has %d lines : %d velocity lines and %d pressure lines\n", n, n_u,n_p);
	PetscPrintf(PETSC_COMM_SELF,"Matrix size : %d x %d, n_u = %d, n_p = %d \n", nrows, ncolumns, n_u, n_p);
	
	ISCreateStride(PETSC_COMM_WORLD, n_u,   0, 1, &is_U);
	ISCreateStride(PETSC_COMM_WORLD, n_p, n_u, 1, &is_P);

	splitPETScMatrix2x2( A_input, n_u, n_p, &M, &G, &D, &C, is_U, is_P);

	buildRHSVectorAndBhat( A_input, n_u, n_p, &X_anal, &b_input, &b_input_p, &b_input_u, &b_hat, is_U, is_P);

	VecDuplicate(b_input,&X_hat);// X_hat will store the numerical solution of the transformed system

	transformSaddlePointMatrix1(M,G,D,C,&A_hat,&Pmat, &C_hat, &G_hat, &diag_2M, &v, n_u);

	// Finalisation of the preconditioner	
	IS is_U_hat,is_P_hat;
	
	ISCreateStride(PETSC_COMM_WORLD, n_u, n_p, 1, &is_U_hat);
	ISCreateStride(PETSC_COMM_WORLD, n_p,   0, 1, &is_P_hat);

//##### Calling KSP solver and monitor convergence
	KSP ksp, *subksp;
	PC pc, subpc0, subpc1;
	KSPType ksp_type = KSPFGMRES, ksp_type0, ksp_type1;
	PCType pc_type=PCFIELDSPLIT, pc_type0, pc_type1;
	int nsplit = 2;
	PCCompositeType pc_composite_type = PC_COMPOSITE_MULTIPLICATIVE;//or ADDITIVE ???

	double residu, abstol, rtol=1e-7, dtol;
	int iter, numberMaxOfIter;

	PetscPrintf(PETSC_COMM_WORLD,"Definition of the solver ...\n");
	KSPCreate(PETSC_COMM_WORLD,&ksp);
	KSPSetType(ksp, ksp_type);
	KSPSetOperators(ksp,A_hat,Pmat);
	KSPSetTolerances(ksp,rtol,PETSC_DEFAULT,PETSC_DEFAULT, PETSC_DEFAULT);
	KSPGetPC(ksp,&pc);
	PetscPrintf(PETSC_COMM_WORLD,"Setting the preconditioner %s...\n", pc_type);
	PCSetType(pc,pc_type);
	if( strcmp(pc_type , PCFIELDSPLIT)==0 ){
		PCFieldSplitSetType(pc, pc_composite_type);
		PCFieldSplitSetIS(pc, "0",is_P_hat);
		PCFieldSplitSetIS(pc, "1",is_U_hat);
	}
	else{
		PetscPrintf(PETSC_COMM_WORLD,"Using PCILU\n");
		PCSetType(pc,PCILU);//This prec works fine in sequential
	}
	PCSetFromOptions(pc);
	PCSetUp(pc);
	KSPSetFromOptions(ksp);
	KSPSetUp(ksp);
	PetscPrintf(PETSC_COMM_WORLD,"Solving the linear system...\n");
	//VecView(X_hat, PETSC_VIEWER_STDOUT_WORLD );
	KSPSolve(ksp,b_hat,X_hat);

	PCFieldSplitGetType(pc, &pc_composite_type);
	KSPGetType(ksp,&ksp_type);
		PCGetType(pc,&pc_type);
	PCFieldSplitSchurGetSubKSP( pc, &nsplit, &subksp);
	KSPGetType(subksp[0],&ksp_type0);
	KSPGetType(subksp[1],&ksp_type1);
	KSPGetPC(subksp[0], &subpc0);
	KSPGetPC(subksp[1], &subpc1);
	PCGetType( subpc0, &pc_type0);
	PCGetType( subpc1, &pc_type1);
	if(pc_composite_type==PC_COMPOSITE_MULTIPLICATIVE)
		PetscPrintf(PETSC_COMM_WORLD,"... linear system solved with ksp_type %s, pc_composite_type PC_COMPOSITE_MULTIPLICATIVE\n",ksp_type);
	else
		PetscPrintf(PETSC_COMM_WORLD,"... linear system solved with ksp_type %s, pc_composite_type %d (different from PC_COMPOSITE_MULTIPLICATIVE)\n",ksp_type,pc_composite_type);

	//Extract informations about the convergence
	KSPConvergedReason reason;
	KSPGetConvergedReason(ksp,&reason);
	KSPGetIterationNumber(ksp,&iter);
	KSPGetResidualNorm( ksp, &residu);
	KSPGetTolerances( ksp, &rtol, &abstol, &dtol, &numberMaxOfIter);

	if (reason>0)
		PetscPrintf(PETSC_COMM_WORLD, "Linear system converged in %d iterations \n", iter);
	else
		PetscPrintf(PETSC_COMM_WORLD, "!!!!!!!!!!!!!!!!!! Linear system diverged  after %d iterations !!!!!!!!!!!!!!\n", iter);
		
	switch(reason){
		case 2:
		    PetscPrintf(PETSC_COMM_WORLD, "Residual 2-norm < rtol*||RHS||_2 with rtol = %e, final residual = %e\n", rtol, residu);
		    break;
		case 3:
		    PetscPrintf(PETSC_COMM_WORLD, "Residual 2-norm < atol with atol = %e, final residual = %e\n", abstol, residu);
		    break;
		case -4:
		    PetscPrintf(PETSC_COMM_WORLD, "!!!!!!! Residual 2-norm > dtol*||RHS||_2 with dtol = %e, final residual = %e !!!!!!! \n", dtol, residu);
		    break;
		case -3:
		    PetscPrintf(PETSC_COMM_WORLD, "!!!!!!! Maximum number of iterations %d reached !!!!!!! \n", numberMaxOfIter);
		    break;
		case -11:
		    PetscPrintf(PETSC_COMM_WORLD, "!!!!!!! Construction of preconditioner failed !!!!!! \n");
		    break;
		case -5:
		    PetscPrintf(PETSC_COMM_WORLD, "!!!!!!! Generic breakdown of the linear solver (Could be due to a singular matrix or preconditioner)!!!!!! \n");
		    break;
		default:
			if (reason>0)
			    PetscPrintf(PETSC_COMM_WORLD, "PETSc convergence reason %d \n", reason);
			else
			    PetscPrintf(PETSC_COMM_WORLD, "PETSc divergence reason %d \n" , reason);
		}

	Vec X_output;
	Vec X_p;//Pressure components of the main unknown
	Vec X_u;//Velocity components of the transformed unknown

	getSolutionFromXhat(G, v, X_hat, &X_output, &X_u, &X_p, is_U_hat, is_P_hat);
	
	error = computeErrorAndCheck( X_anal, X_output, is_U, is_P, X_u, X_p);	
	PetscCheck( error < 1.e-5, PETSC_COMM_WORLD, ierr, "Linear system did not return accurate solution. Error is too high\n");
	
//##### Cleaning of the memory
	MatDestroy(&A_input);
	MatDestroy(&A_hat);
	MatDestroy(&Pmat);
	MatDestroy(&M);
	MatDestroy(&G_hat);
	MatDestroy(&C_hat);
	MatDestroy(&D);	
	MatDestroy(&G);
	MatDestroy(&C);
	MatDestroy(&diag_2M);

	VecDestroy(&b_input);
	VecDestroy(&b_hat);
	VecDestroy(&X_hat);
	VecDestroy(&X_anal);
	VecDestroy(&v);

	ISDestroy(&is_U);
	ISDestroy(&is_P);
	ISDestroy(&is_U_hat);
	ISDestroy(&is_P_hat);

	KSPDestroy(&ksp);
	PetscFree(subksp);
	
	PetscFinalize();
	return ierr;
}
