#include "SaddlePointLinearSolver.h"

//##### Load the matrix A in the file given in the argument
void loadPETScMat(char* file, char* mat_type, Mat * A, int size)
{
	PetscViewer viewer;

	PetscPrintf(PETSC_COMM_WORLD,"Loading Matrix type %s from file %s on %d processor(s)...\n", mat_type, file, size);	

	PetscViewerCreate(PETSC_COMM_WORLD, &viewer);	
	PetscViewerSetType(viewer,PETSCVIEWERBINARY);
	PetscViewerFileSetMode(viewer,FILE_MODE_READ);
	PetscViewerFileSetName(viewer,file);
	
	MatCreate(PETSC_COMM_WORLD, A);
	MatSetType(*A,mat_type);
	MatLoad(*A,viewer);
	PetscViewerDestroy(&viewer);

	PetscPrintf(PETSC_COMM_WORLD,"... matrix Loaded \n");	
}

//####	Decompose the matrix A_input into 4 blocks M, G, D, C
Mat splitPETScMatrix2x2(Mat A_input, PetscInt n_u, PetscInt n_p, Mat * M, Mat * G, Mat *D, Mat * C, IS is_U, IS is_P)
{
	PetscPrintf(PETSC_COMM_WORLD,"Extraction of the 4 blocks \n");
	MatCreateSubMatrix(A_input,is_U, is_U,MAT_INITIAL_MATRIX,M);
	MatCreateSubMatrix(A_input,is_U, is_P,MAT_INITIAL_MATRIX,G);
	MatCreateSubMatrix(A_input,is_P, is_U,MAT_INITIAL_MATRIX,D);
	MatCreateSubMatrix(A_input,is_P, is_P,MAT_INITIAL_MATRIX,C);
	PetscPrintf(PETSC_COMM_WORLD,"... end of extraction\n");
}

//##### Definition of the right hand side to test the preconditioner
void buildRHSVectorAndBhat( Mat A_input, PetscInt n_u, PetscInt n_p, Vec * X_anal, Vec * b_input, Vec * b_input_p, Vec * b_input_u, Vec * b_hat, IS is_U, IS is_P)
{
	Vec X_array[2];
	PetscScalar y_p[n_p], y_u[n_u];
	PetscInt    i_p[n_p], i_u[n_u];

	PetscPrintf(PETSC_COMM_WORLD,"Creation of the RHS, exact and numerical solution vectors...\n");
	VecCreate(PETSC_COMM_WORLD,b_input);
	VecSetSizes(*b_input,PETSC_DECIDE,n_u+n_p);
	VecSetFromOptions(*b_input);

	VecDuplicate(*b_input,X_anal);//X_anal will store the exact solution
	VecDuplicate(*b_input,b_hat);// b_hat will store the right hand side of the transformed system
	
	VecSet(*X_anal,0.0);
	for (int i = n_u;i<n_u+n_p;i++){
		y_p[i-n_u]=1.0/i;
		i_p[i-n_u]=i;
	}
	for (int i = 0;i<n_u;i++){
		y_u[i]=1.0/(i+1);
		i_u[i]=i;
	}
	VecSetValues(*X_anal,n_p,i_p,y_p,INSERT_VALUES);
	//VecSetValues(*X_anal,n_u,i_u,y_u,INSERT_VALUES);
	VecAssemblyBegin(*X_anal);
	VecAssemblyEnd(*X_anal);
	VecNormalize( *X_anal, NULL);
	MatMult( A_input, *X_anal, *b_input);
	PetscPrintf(PETSC_COMM_WORLD,"... vectors created \n");	
	//MatDestroy(&A_input);//Early destruction since A_input is a sequential matrix stored on processed 0

	//Swap the pressure and velocity components + change the sign of the pressure components of b_input (this is due to the change in ordering of the variable in pierre-loic original script)
	VecGetSubVector( *b_input, is_P, b_input_p);
	VecGetSubVector( *b_input, is_U, b_input_u);
	//VecScale(*b_input_p, -1);
	X_array[0] = *b_input_p;
	X_array[1] = *b_input_u;

	//VecCreateNest( PETSC_COMM_WORLD, 2, NULL, X_array, &b_hat);//This may generate an error message : "Nest vector argument 3 not setup "
	VecConcatenate(2, X_array, b_hat, NULL);
}

//##### Application of the transformation A -> A_hat
void transformSaddlePointMatrix1( Mat M, Mat G, Mat D, Mat C, Mat * A_hat, Mat * Pmat, Mat * C_hat, Mat * G_hat, Mat * diag_2M, Vec * v, int n_u)
{
	Mat D_M_inv_G, Mat_array[4];

	Mat_array[3]=M;//Bottom left block of A_hat

	//Extraction of the diagonal of M
	MatCreateVecs(M,NULL,v);//v has the size of M
	MatGetDiagonal(M,*v);

	//Creation of matrix 2*diag(M). Why not use MatCreateDiagonal ???
	MatCreateConstantDiagonal(PETSC_COMM_WORLD, PETSC_DECIDE, PETSC_DECIDE, n_u, n_u, 2, diag_2M);
	MatConvert(*diag_2M,  MATAIJ, MAT_INPLACE_MATRIX, diag_2M);
	MatDiagonalScale(*diag_2M, *v, NULL);//store 2*diagonal part of M
	VecReciprocal(*v);//Must first check that all the coefficients are non zero
	
	// Creation of D_M_inv_G = D_M_inv*G
	MatDuplicate(G,MAT_COPY_VALUES,&D_M_inv_G);//D_M_inv_G contains G
	MatDiagonalScale( D_M_inv_G, *v, NULL);//D_M_inv_G contains D_M_inv*G

	// Creation of C_hat
	MatMatMult(D,D_M_inv_G,MAT_INITIAL_MATRIX,PETSC_DEFAULT,C_hat);//C_hat contains D*D_M_inv*G
	MatAXPY(*C_hat,1.0,C,SUBSET_NONZERO_PATTERN);//C_hat contains C + D*D_M_inv*G
	Mat_array[0]=*C_hat;//Top left block of A_hat

	// Creation of G_hat
	MatMatMult(M,D_M_inv_G,MAT_INITIAL_MATRIX,PETSC_DEFAULT,G_hat);//G_hat contains M*D_M_inv*G
	MatAYPX(*G_hat,-1.0,G,UNKNOWN_NONZERO_PATTERN);//G_hat contains G - M*D_M_inv*G
	Mat_array[2]=*G_hat;//Bottom left block of A_hat

	// Creation of -D
	MatScale(D,-1.0);
	Mat_array[1]=D;//Top right block of A_hat

	// Creation of A_hat = reordered A_input
	MatCreateNest(PETSC_COMM_WORLD,2,NULL,2,NULL,Mat_array,A_hat);

	// Creation of Pmat
	Mat_array[3]=*diag_2M;
	Mat_array[1]=NULL;//Cancel top right block
	MatCreateNest(PETSC_COMM_WORLD,2,NULL,2,NULL,Mat_array,Pmat);

	MatDestroy(&D_M_inv_G);
}

//##### Compute X from X_hat
void getSolutionFromXhat(Mat G, Vec v, Vec X_hat, Vec * X_output, Vec * X_u, Vec * X_p, IS is_U_hat, IS is_P_hat)
{
	Vec X_hat_p;//Pressure components of the transformed unknown
	Vec X_hat_u;//Velocity components of the transformed unknown
	Vec X_array[2];
	
	VecGetSubVector( X_hat, is_P_hat, &X_hat_p);
	VecGetSubVector( X_hat, is_U_hat, &X_hat_u);

	VecDuplicate(X_hat_u,X_u);
	VecDuplicate(X_hat_p,X_p);
	VecCopy(X_hat_p,*X_p);
	MatMult( G, X_hat_p, *X_u);
	VecPointwiseMult(*X_u,*X_u,v);
	VecAYPX( *X_u, -1, X_hat_u)	;

	X_array[0] = *X_u;
	X_array[1] = *X_p;

	//VecCreateNest( PETSC_COMM_WORLD, 2, NULL, X_array, &X_output);//This generate an error message : "Nest vector argument 3 not setup "
	VecConcatenate(2, X_array, X_output, NULL);
}

//##### Compute the error and check it is small
double computeErrorAndCheck( Vec X_anal, Vec X_output, IS is_U, IS is_P, Vec X_u, Vec X_p)
{	
	Vec X_anal_p, X_anal_u;//Pressure and velocity components of the analitic solution
	double error, error_p, error_u;
	
	VecGetSubVector( X_anal, is_P, &X_anal_p);
	VecGetSubVector( X_anal, is_U, &X_anal_u);

	VecAXPY(  X_p, -1, X_anal_p);
	VecNorm(  X_p, NORM_2, &error_p);
	PetscPrintf(PETSC_COMM_WORLD,"L2 Error p : ||X_anal_p - X_num_p|| = %e\n", error_p);
	VecAXPY(  X_u, -1, X_anal_u);
	VecNorm(  X_u, NORM_2, &error_u);
	PetscPrintf(PETSC_COMM_WORLD,"L2 Error u : ||X_anal_u - X_num_u|| = %e \n", error_u);

	VecAXPY(X_output, -1, X_anal);
	VecNorm( X_output, NORM_2, &error);
	PetscPrintf(PETSC_COMM_WORLD,"L2 Error : ||X_anal - X_num|| = %e, (remember ||X_anal||=1)\n", error);

	return error;
}
