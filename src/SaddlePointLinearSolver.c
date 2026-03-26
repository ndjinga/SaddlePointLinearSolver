#include "SaddlePointLinearSolver.h"

//##### Load the matrix A from the file given in the argument
void loadPETScMat(char* file, char* mat_type, Mat * A, PetscInt n_u, PetscInt n_p)// n_u (resp. n_p) is the number of velocity (resp. pressure) lines in the matrix, used only to optimise the parallel distribution of the matrix.
{
	PetscMPIInt    size;        /* size of communicator */
	PetscMPIInt    rank;        /* processor rank */
	MPI_Comm_rank(PETSC_COMM_WORLD,&rank);
	MPI_Comm_size(PETSC_COMM_WORLD,&size);
	PetscViewer viewer;
	PetscInt n = n_u + n_p;
	
	PetscPrintf(PETSC_COMM_WORLD,"Loading Matrix type %s from file %s on %d processor(s)...\n", mat_type, file, size);	

	PetscViewerCreate(PETSC_COMM_WORLD, &viewer);	
	PetscViewerSetType(viewer,PETSCVIEWERBINARY);
	PetscViewerFileSetMode(viewer,FILE_MODE_READ);
	PetscViewerFileSetName(viewer,file);
	
	MatCreate(PETSC_COMM_WORLD, A);
	MatSetType(*A,mat_type);

	if( size>1)
	    if( rank == 0)
	        MatSetSizes( *A, n-(size-1)*((n-n_u)/(size-1)), n-(size-1)*((n-n_u)/(size-1)), n, n);
	    else
	        MatSetSizes( *A, (n-n_u)/(size-1), (n-n_u)/(size-1), n, n);

	MatLoad(*A,viewer);

	PetscPrintf(PETSC_COMM_WORLD,"... matrix Loaded \n");	

	PetscViewerDestroy(&viewer);
}

//####	Decompose the matrix A_input into 4 blocks M, G, D, C
int splitPETScMatrix2x2(Mat A_input, PetscInt n_u, PetscInt n_p, Mat * M, Mat * G, Mat *D, Mat * C, IS * is_U, IS * is_P)
{
	PetscMPIInt    size;        /* size of communicator */
	PetscMPIInt    rank;        /* processor rank */
	MPI_Comm_rank(PETSC_COMM_WORLD,&rank);
	MPI_Comm_size(PETSC_COMM_WORLD,&size);
	PetscInt nrows, ncolumns;//Total number of rows and columns of A_input
	PetscInt irow_min, irow_max;//min and max indices of rows stored locally on this process

	MatGetOwnershipRange( A_input, &irow_min, &irow_max);
	MatGetSize( A_input, &nrows, &ncolumns);
	PetscInt min_pressure_lines = irow_min <= n_u ? n_u : irow_min;//max(irow_min, n_u)
	PetscInt max_velocity_lines = irow_max >= n_u ? n_u : irow_max;//min(irow_max, n_u)
	PetscInt nb_pressure_lines = irow_max >= n_u ? irow_max - min_pressure_lines : 0;
	PetscInt nb_velocity_lines = irow_min <= n_u ? max_velocity_lines - irow_min : 0;
	PetscInt nb_local_lines = irow_max - irow_min; 

	PetscCheck( nrows == ncolumns, PETSC_COMM_WORLD, PETSC_ERR_ARG_SIZ, "Matrix is not square !!!\n");
	PetscCheck( n_u+n_p == ncolumns, PETSC_COMM_WORLD, PETSC_ERR_ARG_SIZ, "Inconsistent data : the matrix has %d lines but only %d velocity lines and %d pressure lines declared\n", ncolumns, n_u,n_p);
	PetscPrintf(PETSC_COMM_WORLD,"The matrix has %d lines : %d velocity lines and %d pressure lines\n", n_u+n_p, n_u,n_p);
	PetscPrintf(PETSC_COMM_SELF,"Process %d has %d local rows : irow_min = %d, irow_max = %d, min_pressure_lines = %d, max_velocity_lines = %d, nb_pressure_lines = %d, nb_velocity_lines = %d \n", rank, nb_local_lines, irow_min, irow_max, min_pressure_lines, max_velocity_lines, nb_pressure_lines, nb_velocity_lines);
	
	PetscPrintf(PETSC_COMM_WORLD,"Extraction of the 4 blocks \n M G\n D C\n");
	ISCreateStride(PETSC_COMM_WORLD, nb_velocity_lines, max_velocity_lines - nb_velocity_lines, 1, is_U);
	ISCreateStride(PETSC_COMM_WORLD, nb_pressure_lines, min_pressure_lines                    , 1, is_P);

	MatCreateSubMatrix(A_input,*is_U, *is_U,MAT_INITIAL_MATRIX,M);
	MatCreateSubMatrix(A_input,*is_U, *is_P,MAT_INITIAL_MATRIX,G);
	MatCreateSubMatrix(A_input,*is_P, *is_U,MAT_INITIAL_MATRIX,D);
	MatCreateSubMatrix(A_input,*is_P, *is_P,MAT_INITIAL_MATRIX,C);
	PetscPrintf(PETSC_COMM_WORLD,"... end of extraction\n");

	//#Display some informations about the four blocs
	MatGetOwnershipRange( *M, &irow_min, &irow_max);
	PetscPrintf(PETSC_COMM_SELF,"Matrix M, Process %d local rows : irow_min = %d, irow_max = %d \n", rank, irow_min, irow_max);
	MatGetOwnershipRange( *G, &irow_min, &irow_max);
	PetscPrintf(PETSC_COMM_SELF,"Matrix G, Process %d local rows : irow_min = %d, irow_max = %d \n", rank, irow_min, irow_max);
	MatGetOwnershipRange( *D, &irow_min, &irow_max);
	PetscPrintf(PETSC_COMM_SELF,"Matrix D, Process %d local rows : irow_min = %d, irow_max = %d \n", rank, irow_min, irow_max);
	MatGetOwnershipRange( *C, &irow_min, &irow_max);
	PetscPrintf(PETSC_COMM_SELF,"Matrix C, Process %d local rows : irow_min = %d, irow_max = %d \n", rank, irow_min, irow_max);

	int size1, size2;
	MatGetSize(*M, &size1,&size2);
	PetscPrintf(PETSC_COMM_WORLD,"Size of M : %d,%d\n", size1,size2);
	MatGetSize(*G, &size1,&size2);
	PetscPrintf(PETSC_COMM_WORLD,"Size of G : %d,%d\n", size1,size2);
	MatGetSize(*D, &size1,&size2);
	PetscPrintf(PETSC_COMM_WORLD,"Size of D : %d,%d\n", size1,size2);
	MatGetSize(*C, &size1,&size2);
	PetscPrintf(PETSC_COMM_WORLD,"Size of C : %d,%d\n", size1,size2);
}

//##### Definition of the right hand side to test the preconditioner
void buildRHSVector( Mat A_input, PetscInt n_u, PetscInt n_p, Vec * X_anal, Vec * b_input)
{
	PetscMPIInt    size;        /* size of communicator */
	PetscMPIInt    rank;        /* processor rank */
	MPI_Comm_rank(PETSC_COMM_WORLD,&rank);
	MPI_Comm_size(PETSC_COMM_WORLD,&size);
	PetscInt irow_min, irow_max;//min and max indices of rows stored locally on this process
	MatGetOwnershipRange( A_input, &irow_min, &irow_max);

	PetscScalar *values;//Pointer to the storage of the vector values
	PetscInt    *indices;//To store the indices
	PetscMalloc1(irow_max-irow_min, &values);
	PetscMalloc1(irow_max-irow_min, &indices);
	 
	PetscPrintf(PETSC_COMM_WORLD,"Creation of the RHS, exact and numerical solution vectors...\n");
	MatCreateVecs( A_input, b_input, X_anal );// parallel distribution of vectors should optimise the computation A_input*X_anal=b_input
	
	for (int i = 0; i<irow_max-irow_min; i++){
		values[i] = 1.0/(irow_min+i+1);//valeur second membre à imposer ici
		indices[i]=irow_min+i;
	}
	
	VecSetValues(*X_anal,irow_max-irow_min,indices,values,INSERT_VALUES);
	VecAssemblyBegin(*X_anal);
	VecAssemblyEnd(*X_anal);
	VecNormalize( *X_anal, NULL);
	MatMult( A_input, *X_anal, *b_input);

	PetscPrintf(PETSC_COMM_WORLD,"... vectors created \n");	
	
	PetscFree(values);
	PetscFree(indices);
}

//##### Application of the transformation A -> A_hat
void transformSaddlePointMatrix( Mat M, Mat G, Mat D, Mat C, Mat * A_hat, Mat * Pmat, Vec * v)
{
	PetscPrintf(PETSC_COMM_WORLD,"Transformation of the original system matrix...\n");

	Vec v_redistributed;
	Mat D_M_inv_G, C_hat, G_hat, diag_2M, Mat_array[4];// D_M_inv = diag(M)^{-1}
	VecScatter scat;//tool to redistribute a vector on the processors
	IS is_to, is_from;
	PetscInt col_min, col_max;

	//Extraction of the diagonal of M
	MatCreateVecs(M,NULL,v);//v has the size of M
	MatGetDiagonal(M,*v);

	//Creation of matrix 2*diag(M). Why not use MatCreateDiagonal ???
	MatDuplicate(M, MAT_DO_NOT_COPY_VALUES, &diag_2M);
	MatEliminateZeros(diag_2M, PETSC_TRUE);
	MatDiagonalSet(diag_2M, *v,  INSERT_VALUES);
	MatScale(diag_2M,2);//store 2*diagonal part of M
	VecReciprocal(*v);//Must first check that all the coefficients are non zero
	
	// Creation of D_M_inv_G = D_M_inv*G = diag(M)^{-1} * G
	MatDuplicate(G,MAT_COPY_VALUES,&D_M_inv_G);//D_M_inv_G contains G
	MatCreateVecs(D_M_inv_G,NULL,&v_redistributed);//v_redistributed has the parallel distribution of D_M_inv_G
	VecGetOwnershipRange(*v,&col_min,&col_max);
	ISCreateStride(PETSC_COMM_WORLD, col_max-col_min, col_min, 1, &is_from);
	VecGetOwnershipRange(v_redistributed,&col_min,&col_max);
	ISCreateStride(PETSC_COMM_WORLD, col_max-col_min, col_min, 1, &is_to);
	VecScatterCreate(*v,is_from,v_redistributed,is_to,&scat);
	VecScatterBegin(scat, *v, v_redistributed,INSERT_VALUES,SCATTER_FORWARD);
	VecScatterEnd(  scat, *v, v_redistributed,INSERT_VALUES,SCATTER_FORWARD);
	MatDiagonalScale( D_M_inv_G, v_redistributed, NULL);//D_M_inv_G contains D_M_inv*G

	// Creation of C_hat
	MatMatMult(D,D_M_inv_G,MAT_INITIAL_MATRIX,PETSC_DEFAULT,&C_hat);//C_hat contains D*D_M_inv*G
	MatAYPX(C_hat,-1.0,C,SUBSET_NONZERO_PATTERN);//C_hat contains C - D*D_M_inv*G

	// Creation of G_hat
	MatMatMult(M,D_M_inv_G,MAT_INITIAL_MATRIX,PETSC_DEFAULT,&G_hat);//G_hat contains M*D_M_inv*G
	MatAYPX(G_hat,-1.0,G,UNKNOWN_NONZERO_PATTERN);//G_hat contains G - M*D_M_inv*G

	//Creation of global matrices using MatCreateNest
	Mat_array[3]=C_hat;//Top left block of A_hat
	Mat_array[2]=D;//Top right block of A_hat
	Mat_array[1]=G_hat;//Bottom left block of A_hat
	Mat_array[0]=M;//Bottom left block of A_hat

	// Creation of A_hat = reordered A_input
	MatCreateNest(PETSC_COMM_WORLD,2,NULL,2,NULL,Mat_array,A_hat);

	// Creation of Pmat
	Mat_array[0]=diag_2M;
	MatCreateNest(PETSC_COMM_WORLD,2,NULL,2,NULL,Mat_array,Pmat);

	PetscPrintf(PETSC_COMM_WORLD,"... matrix transformed \n");	

	MatDestroy(&D_M_inv_G);
	VecScatterDestroy(&scat);
	VecDestroy(&v_redistributed);
}

//##### Compute X from X_hat
void getSolutionFromXhat(Mat G, Vec v, Vec X_hat, Vec * X_output, Vec * X_u, Vec * X_p, IS is_U, IS is_P)
{
	Vec X_hat_p;//Pressure components of the transformed unknown
	Vec X_hat_u;//Velocity components of the transformed unknown
	Vec X_array[2];
	IS IS_array[2];
	
	VecGetSubVector( X_hat, is_P, &X_hat_p);
	VecGetSubVector( X_hat, is_U, &X_hat_u);

	VecDuplicate(X_hat_u,X_u);
	VecDuplicate(X_hat_p,X_p);
	VecCopy(X_hat_p,*X_p);
	MatMult( G, X_hat_p, *X_u);
	VecPointwiseMult(*X_u,*X_u,v);
	VecAYPX( *X_u, -1, X_hat_u)	;

	X_array[0] = *X_u;
	X_array[1] = *X_p;
	IS_array[0] = is_U;
	IS_array[1] = is_P;
	
	VecCreateNest( PETSC_COMM_WORLD, 2, IS_array, X_array, X_output);
	//VecConcatenate(2, X_array, X_output, NULL);//Works only in sequential mode
}

//##### Compute the error and check it is small
double computeErrorAndCheck( Vec X_anal, Vec X_output, IS is_U, IS is_P, Vec X_u, Vec X_p)
{	
	Vec X_anal_p, X_anal_u;//Pressure and velocity components of the analytic solution
	double error, error_p, error_u;
	
	VecGetSubVector( X_anal, is_P, &X_anal_p);
	VecGetSubVector( X_anal, is_U, &X_anal_u);

	VecAXPY(  X_p, -1, X_anal_p);
	VecNorm(  X_p, NORM_2, &error_p);
	PetscPrintf(PETSC_COMM_WORLD,"L2 Error p : ||X_anal_p - X_num_p|| = %e\n", error_p);
	VecAXPY(  X_u, -1, X_anal_u);
	VecNorm(  X_u, NORM_2, &error_u);
	PetscPrintf(PETSC_COMM_WORLD,"L2 Error u : ||X_anal_u - X_num_u|| = %e \n", error_u);

	//VecAXPY(X_output, -1, X_anal);//This generates an error "Incompatible vector local lengths parameter" probably because Xoutput is a nest vector and Xanal is not
	//VecNorm( X_output, NORM_2, &error);
	error=sqrt(error_u*error_u+error_p*error_p);
	PetscPrintf(PETSC_COMM_WORLD,"L2 Error : ||X_anal - X_num|| = %e, (remember ||X_anal||=1)\n", error);

	PetscCheck( error < 1e-4, PETSC_COMM_WORLD, PETSC_ERR_NOT_CONVERGED, "Linear system did not return accurate solution. Error is too high compared to residual (e>1e-4) : e=%e\n", error);

	return error;
}
