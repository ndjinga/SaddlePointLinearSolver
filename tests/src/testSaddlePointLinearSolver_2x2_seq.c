static char help[] = "Read a PETSc matrix from a file -f0 <input file>\n Parameters : \n -f0 : matrix fileName \n -nU :number of velocity lines \n -nP : number of pressure lines \n -mat_type : PETSc matrix type \n -usePrec : boolean yes or no (default is yes) \n";

/*************************************************************************************************/
/* Sequential implementation of a transform-then-solve preconditioner for the linear system A_{input} X_{output} = b_{input} */
/*            Find a block triangular matrix T and perform the change of variables  X_hat = T^{-1}X, A_hat = (A_{input}T)T^{-1} */
/*            Pressure and velocity unknowns are swaped in the transform for convenience reasons */ 
/*                                                                                               */
/* Description : Sequential file with PC_COMPOSITE of MULTIPLICATIVE type, not restricted to 2x2 blocs.*/
/*               Use of API (class SaddlePointLinearSolve) for better code factorisation         */ 
/*               Performance is good even when RHS bloc pressure is empty unlike Shat approach   */ 
/*                                                                                               */
/* Input  : - Matrix A_{input}    (system matrix, loaded from a file)                            */
/*          - Vector b_{input}    (right hand side, made up for testing)                         */
/* Output : - Vector X_{output}   (unknown vector, to be determined)                              */
/*                                                                                               */
/* Auxilliary variables : - A_hat (transformed matrix)                                           */
/*                        - X_hat (unknown of the transformed system)                            */
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
/*                                 *M     G_hat*             G_hat=G - M*D_M_inv*G               */
/*                        A_hat = *             *                                                */
/*                                 *D     C_hat*             C_hat=C - D*D_M_inv*G               */
/*                                                                                               */
/*                                 *2 diag(M)     0  *                                           */
/*                        Pmat  = *                   *                                          */
/*                                 *D          C_hat *                                           */
/*                                                                                               */
/*************************************************************************************************/

#include "SaddlePointLinearSolver.h"

int main( int argc, char **args ){
	PetscInitialize(&argc,&args, (char*)0,help);
	PetscMPIInt    size;        /* size of communicator */
	PetscMPIInt    rank;        /* processor rank */
	MPI_Comm_rank(PETSC_COMM_WORLD,&rank);
	MPI_Comm_size(PETSC_COMM_WORLD,&size);
	PetscInt n_u, n_p, n;//Total number of velocity and pressure lines. n = n_u+ n_p
	char file[1][PETSC_MAX_PATH_LEN], mat_type[256]; // File to load, matrix type
	Mat A_input, A_hat, Pmat;
	Mat M, G, D, C;
	Mat C_hat, G_hat, diag_2M;
	IS is_U,is_P;
	Vec b_input, X_hat, X_anal;
	Vec v;
	double error;
	
	PetscCheck( size == 1, PETSC_COMM_WORLD, PETSC_ERR_ARG_SIZ, "Incorrect number of procs nprocs = %d.\n !!! This is a sequential implementation !!! \n", size);

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

	VecDuplicate(b_input,&X_hat);// X_hat will store the numerical solution of the transformed system

	transformSaddlePointMatrix(M,G,D,C,&A_hat,&Pmat, &C_hat, &G_hat, &diag_2M, &v, n_u);


//##### Calling KSP solver and monitor convergence
	KSP ksp, *kspArray;
	PC pc, pc1, pc2;
	KSPType ksp_type = KSPFGMRES, ksp_type0, ksp_type1;
	PCType pc_type=PCFIELDSPLIT, pc_type0, pc_type1;
	int nblocks=2;
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
		PCFieldSplitSetIS(pc, "0",is_P);
		PCFieldSplitSetIS(pc, "1",is_U);
		PCFieldSplitGetSubKSP( pc, &nblocks, &kspArray);
		KSPSetType( kspArray[0], KSPPREONLY);
		KSPSetType( kspArray[1], KSPPREONLY);
		KSPGetPC(kspArray[0], &pc1);
		KSPGetPC(kspArray[1], &pc2);

		PetscBool usePrec = PETSC_TRUE;
		PetscOptionsGetBool( NULL, NULL, "-usePrec", &usePrec, NULL);
		if (usePrec )
		{
		    PCSetType( pc1, PCJACOBI);
		    PCSetType( pc2, PCGAMG);
		}
		else
		{
		    PCSetType( pc1, PCNONE);
		    PCSetType( pc2, PCNONE);
		}
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
	KSPSolve(ksp,b_input,X_hat);

	PCFieldSplitGetType(pc, &pc_composite_type);
	KSPGetType(ksp,&ksp_type);
	PCGetType(pc,&pc_type);
	PCFieldSplitSchurGetSubKSP( pc, &nblocks, &kspArray);
	KSPGetType(kspArray[0],&ksp_type0);
	KSPGetType(kspArray[1],&ksp_type1);
	KSPGetPC(kspArray[0], &pc1);
	KSPGetPC(kspArray[1], &pc2);
	PCGetType( pc1, &pc_type0);
	PCGetType( pc2, &pc_type1);
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
		    PetscPrintf(PETSC_COMM_WORLD, "Residual 2-norm < rtol*||RHS||_2 with rtol = %e, final residual = %e\n\n", rtol, residu);
		    break;
		case 3:
		    PetscPrintf(PETSC_COMM_WORLD, "Residual 2-norm < atol with atol = %e, final residual = %e\n\n", abstol, residu);
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

	getSolutionFromXhat(G, v, X_hat, &X_output, &X_u, &X_p, is_U, is_P);
	
	error = computeErrorAndCheck( X_anal, X_output, is_U, is_P, X_u, X_p);	
	PetscCheck( error < 1e6*residu, PETSC_COMM_WORLD, PETSC_ERR_NOT_CONVERGED, "Linear system did not return accurate solution. Error is too high compared to residual (e>1e6*r) : e=%e, r=%e\n", error, residu);
	
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
	VecDestroy(&X_hat);
	VecDestroy(&X_anal);
	VecDestroy(&v);

	ISDestroy(&is_U);
	ISDestroy(&is_P);

	KSPDestroy(&ksp);
	PetscFree(kspArray);
	
	PetscFinalize();
	return 0;
}
