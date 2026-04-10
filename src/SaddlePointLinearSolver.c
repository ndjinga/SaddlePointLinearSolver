#include "SaddlePointLinearSolver.h"

//##### Load the matrix A from the file given in the argument
//### Creation of Mat A. Mat A must be deleted by caller
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

//####    Decompose the matrix A_input into 4 blocks M, G, D, C
//### M, G, D, C, is_U and is_P must be deleted by caller
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
//### Vectors X_anal and b_input must be deleted by caller
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

//##### Application of the transformation A -> A_hat by multiplication to the right by an upper triangular matrix
//## Vector v must be deleted by caller
/*                                 *M   G*                                                       */
/*                        A     = *       *                                                      */
/*                                 *D   C*                                                       */
/*                                                                                               */
/*                                 *Id  -diag(M)^{-1}G*                    *Id  diag(M)^{-1}G*   */
/*                        U     = *                    *         U^{-1} = *                   *  */
/*                                 *0               Id*                    *0              Id*   */
/*                                                                                               */
/*                                 *M     G_hat*             G_hat=G - M*diag(M)^{-1}*G          */
/*                        A_hat = *             *                                                */
/*                                 *D     C_hat*             C_hat=C - D*diag(M)^{-1}*G          */
/*                                                                                               */
/*                                 *2 diag(M)     0  *                                           */
/*                        Pmat  = *                   *                                          */
/*                                 *D          C_hat *                                           */
/*                                                                                               */
/*************************************************************************************************/
int transformSystemRight( Mat M, Mat G, Mat D, Mat C, Mat * A_hat, Mat * Pmat, Mat * C_hat, Mat * G_hat, Mat * diag_2M, Vec * v)
{
    PetscPrintf(PETSC_COMM_WORLD,"Transformation of the original system matrix by multiplication to the right...\n");

    Vec v_redistributed;
    Mat D_M_inv_G, Mat_array[4];// D_M_inv = diag(M)^{-1}
    VecScatter scat;//tool to redistribute a vector on the processors
    IS is_to, is_from;
    PetscInt col_min, col_max;

    //Extraction of the diagonal of M
    MatCreateVecs(M,NULL,v);//v has the size of M
    MatGetDiagonal(M,*v);

    //Creation of matrix 2*diag(M). Why not use MatCreateDiagonal ???
    PetscCall( MatDuplicate(M, MAT_DO_NOT_COPY_VALUES, diag_2M) );
    MatEliminateZeros(*diag_2M, PETSC_TRUE);
    MatDiagonalSet(*diag_2M, *v,  INSERT_VALUES);
    MatScale(*diag_2M,2);//store 2*diagonal part of M
    PetscCall( VecReciprocal(*v) );//Must first check that all the coefficients are non zero
    
    // Creation of D_M_inv_G = D_M_inv*G = diag(M)^{-1} * G
    PetscCall( MatDuplicate(G,MAT_COPY_VALUES,&D_M_inv_G) );//D_M_inv_G contains G
    PetscCall( MatCreateVecs(D_M_inv_G,NULL,&v_redistributed) );//v_redistributed has the parallel distribution of D_M_inv_G
    VecGetOwnershipRange(*v,&col_min,&col_max);
    ISCreateStride(PETSC_COMM_WORLD, col_max-col_min, col_min, 1, &is_from);
    VecGetOwnershipRange(v_redistributed,&col_min,&col_max);
    ISCreateStride(PETSC_COMM_WORLD, col_max-col_min, col_min, 1, &is_to);
    VecScatterCreate(*v,is_from,v_redistributed,is_to,&scat);
    VecScatterBegin(scat, *v, v_redistributed,INSERT_VALUES,SCATTER_FORWARD);
    VecScatterEnd(  scat, *v, v_redistributed,INSERT_VALUES,SCATTER_FORWARD);
    MatDiagonalScale( D_M_inv_G, v_redistributed, NULL);//D_M_inv_G contains D_M_inv*G

    // Creation of C_hat
    MatMatMult(D,D_M_inv_G,MAT_INITIAL_MATRIX,PETSC_DEFAULT,C_hat);//C_hat contains D*D_M_inv*G
    MatAYPX(*C_hat,-1.0,C,SUBSET_NONZERO_PATTERN);//C_hat contains C - D*D_M_inv*G

    // Creation of G_hat
    MatMatMult(M,D_M_inv_G,MAT_INITIAL_MATRIX,PETSC_DEFAULT,G_hat);//G_hat contains M*D_M_inv*G
    MatAYPX(*G_hat,-1.0,G,UNKNOWN_NONZERO_PATTERN);//G_hat contains G - M*D_M_inv*G

    //Creation of global matrices using MatCreateNest
    Mat_array[3]=*C_hat;//Top left block of A_hat
    Mat_array[2]=D;//Top right block of A_hat
    Mat_array[1]=*G_hat;//Bottom left block of A_hat
    Mat_array[0]=M;//Bottom left block of A_hat

    // Creation of A_hat = reordered A_input
    MatCreateNest(PETSC_COMM_WORLD,2,NULL,2,NULL,Mat_array,A_hat);

    // Creation of Pmat
    Mat_array[0]=*diag_2M;//Replace M by its diagonal to ease inversion
    MatCreateNest(PETSC_COMM_WORLD,2,NULL,2,NULL,Mat_array,Pmat);

    PetscPrintf(PETSC_COMM_WORLD,"... matrix transformed \n");    

    MatDestroy(diag_2M);
    MatDestroy(C_hat);
    MatDestroy(G_hat);
    MatDestroy(&D_M_inv_G);
    VecScatterDestroy(&scat);
    VecDestroy(&v_redistributed);
}

//##### Application of the transformation A -> A_hat (and b -> b_hat) by multiplication to the left by a lower triangular matrix
//## Vector v must be deleted by caller
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
int transformSystemLeft( Mat M, Mat G, Mat D, Mat C, Mat * A_hat, Mat * Pmat, Mat * C_hat, Mat * D_hat, Mat * diag_2M, Vec * v, PetscBool useLowerTriangularTransform)
{
    PetscPrintf(PETSC_COMM_WORLD,"Transformation of the original system matrix by multiplication to the left...\n");

    Vec v_redistributed;
    Mat D_DM_inv, Mat_array[4];// D_DM_inv = D*diag(M)^{-1}
    VecScatter scat;//tool to redistribute a vector on the processors
    IS is_to, is_from;
    PetscInt col_min, col_max;

    //Extraction of the diagonal of M
    MatCreateVecs(M,NULL,v);//v has the size of M
    MatGetDiagonal(M,*v);

    //Creation of matrix 2*diag(M). Why not use MatCreateDiagonal ???
    PetscCall( MatDuplicate(M, MAT_DO_NOT_COPY_VALUES, diag_2M) );
    MatEliminateZeros(*diag_2M, PETSC_TRUE);
    MatDiagonalSet(*diag_2M, *v,  INSERT_VALUES);
    MatScale(*diag_2M,2);//store 2*diagonal part of M
    PetscCall( VecReciprocal(*v) );//Must first check that all the coefficients are non zero
    
    // Creation of D_DM_inv = D*DM_inv = D*diag(M)^{-1}
    PetscCall( MatDuplicate(D,MAT_COPY_VALUES,&D_DM_inv) );//D_DM_inv contains D
    PetscCall( MatCreateVecs(D_DM_inv,&v_redistributed,NULL) );//v_redistributed has the parallel distribution of D_DM_inv
    VecGetOwnershipRange(*v,&col_min,&col_max);
    ISCreateStride(PETSC_COMM_WORLD, col_max-col_min, col_min, 1, &is_from);
    VecGetOwnershipRange(v_redistributed,&col_min,&col_max);
    ISCreateStride(PETSC_COMM_WORLD, col_max-col_min, col_min, 1, &is_to);
    VecScatterCreate(*v,is_from,v_redistributed,is_to,&scat);
    VecScatterBegin(scat, *v, v_redistributed,INSERT_VALUES,SCATTER_FORWARD);
    VecScatterEnd(  scat, *v, v_redistributed,INSERT_VALUES,SCATTER_FORWARD);
    MatDiagonalScale( D_DM_inv, NULL, v_redistributed);//D_DM_inv contains D_DM_inv

    // Creation of C_hat
    MatMatMult(D_DM_inv,G,MAT_INITIAL_MATRIX,PETSC_DEFAULT,C_hat);//C_hat contains D*D_M_inv*G
    MatAYPX(*C_hat,-1.0,C,SUBSET_NONZERO_PATTERN);//C_hat contains C - D*D_M_inv*G

    // Creation of D_hat
    MatMatMult(D_DM_inv,M,MAT_INITIAL_MATRIX,PETSC_DEFAULT,D_hat);//D_hat contains D*DM_inv*M
    MatAYPX(*D_hat,-1.0,D,UNKNOWN_NONZERO_PATTERN);//D_hat contains D - D*DM_inv*M

    //Creation of global matrices using MatCreateNest
    if( useLowerTriangularTransform )//Default, faster
    {
        Mat_array[0]=*C_hat;//Top left block of A_hat
        Mat_array[1]=*D_hat;//Top right block of A_hat
        Mat_array[3]=M;//Bottom left block of A_hat
        Mat_array[2]=G;//Bottom left block of A_hat
    }
    else
    {
        Mat_array[3]=*C_hat;
        Mat_array[2]=*D_hat;
        Mat_array[0]=M;
        Mat_array[1]=G;
    }

    // Creation of A_hat = reordered A_input
    MatCreateNest(PETSC_COMM_WORLD,2,NULL,2,NULL,Mat_array,A_hat);

    // Creation of Pmat
    if( useLowerTriangularTransform )
        Mat_array[3]=*diag_2M;//Replace M by its diagonal to ease inversion
    else
        Mat_array[0]=*diag_2M;//Replace M by its diagonal to ease inversion
    MatCreateNest(PETSC_COMM_WORLD,2,NULL,2,NULL,Mat_array,Pmat);

    PetscPrintf(PETSC_COMM_WORLD,"... matrix transformed \n");    

    MatDestroy(diag_2M);
    MatDestroy(C_hat);
    MatDestroy(D_hat);
    MatDestroy(&D_DM_inv);
    VecScatterDestroy(&scat);
    VecDestroy(&v_redistributed);
}

//##### Compute X from X_hat
//## Vectors X_output, X_u, X_p must be deleted by caller
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
    VecAYPX( *X_u, -1, X_hat_u)    ;

    X_array[0] = *X_u;
    X_array[1] = *X_p;
    IS_array[0] = is_U;
    IS_array[1] = is_P;
    
    VecCreateNest( PETSC_COMM_WORLD, 2, IS_array, X_array, X_output);
    //VecConcatenate(2, X_array, X_output, NULL);//Works only in sequential mode

    VecDestroy(&X_hat_u);
    VecDestroy(&X_hat_p);
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

    PetscCheck( error < 1e-4, PETSC_COMM_WORLD, PETSC_ERR_NOT_CONVERGED, "Linear system did not return accurate solution. Error ||X-Xanal|| is too high (||X-Xanal||>1e-4) : ||X-Xanal||=%e\n", error);

    return error;
}

//##### Build right hand side b_hat for left preconditioned system
//User should destroy b_hat after use
int getbhatFrombinput(Mat D, Vec v, Vec b_input, Vec * b_hat, IS is_U, IS is_P, PetscBool useLowerTriangularTransform)
{
    Vec b_hat_u;//Velocity components of the transformed unknown
    Vec b_hat_p;//Pressure components of the main unknown
    Vec b_hat_u_tmp;//Temporary storage for velocity components
    Vec b_hat_p_tmp;//Temporary storage for pressure components
    Vec b_array[2];
    
    PetscCall( VecDuplicate(b_input,b_hat) );// b_hat will store the right hand side of the transformed system
    PetscCall( VecGetSubVector( b_input, is_P, &b_hat_p) );
    PetscCall( VecGetSubVector( b_input, is_U, &b_hat_u) );

    PetscCall( VecDuplicate(b_hat_u,&b_hat_u_tmp) );
    PetscCall( VecDuplicate(b_hat_p,&b_hat_p_tmp) );
    PetscCall( VecPointwiseMult(b_hat_u_tmp,b_hat_u,v) );

    PetscCall( MatMult( D, b_hat_u_tmp, b_hat_p_tmp) );
    PetscCall( VecAXPY( b_hat_p, -1, b_hat_p_tmp) );

    if( useLowerTriangularTransform )
    {
        b_array[0] = b_hat_p;
        b_array[1] = b_hat_u;
    }
    else
    {
        b_array[0] = b_hat_u;
        b_array[1] = b_hat_p;
    }

    //VecCreateNest( PETSC_COMM_WORLD, 2, NULL, b_array, &b_hat);//This generate an error message : "Nest vector argument 3 not setup "
    PetscCall( VecConcatenate(2, b_array, b_hat, NULL) );

    VecDestroy(&b_hat_u_tmp);
    VecDestroy(&b_hat_p_tmp);
}

//##### Solve the right transformed system for Xhat
int solveRightTransformedSystemForXhat( Mat A_hat, Mat Pmat, IS is_U, IS is_P, Vec b_input, Vec * X_hat, PetscReal rtol, PetscReal abstol, PetscReal dtol, PetscInt numberMaxOfIter, double *residu)
{
    KSP ksp, *kspArray;
    PC pc, pc1, pc2;
    KSPType ksp_type0, ksp_type1,  ksp_type = KSPFBCGS;//BCGS seems very efficient
    PCType pc_type=PCFIELDSPLIT, pc_type0, pc_type1;
    int nblocks=2, iter, iter1, iter2;//iter = main iteration number, iter1 and iter2 are sub iteration numbers
    PCCompositeType pc_composite_type = PC_COMPOSITE_MULTIPLICATIVE;// MULTIPLICATIVE = block triangular preconditioner, ADDITIVE  = block diagonal preconditioner

    PetscPrintf(PETSC_COMM_WORLD,"Setting the solver ...\n");
    KSPCreate(PETSC_COMM_WORLD,&ksp);
    KSPSetType(ksp, ksp_type);
    PetscCall( KSPSetOperators(ksp,A_hat,Pmat) );
    KSPSetTolerances(ksp,rtol, abstol, dtol, numberMaxOfIter);
    KSPGetPC(ksp,&pc);
    PetscPrintf(PETSC_COMM_WORLD,"Setting the preconditioner %s...\n", pc_type);
    PCSetType(pc,pc_type);

    PCFieldSplitSetType(pc, pc_composite_type);
    PCFieldSplitSetIS(pc, "0",is_U);//The order here matters a lot between this line and the next
    PCFieldSplitSetIS(pc, "1",is_P);//The order here matters a lot between this line and the previous
    PCFieldSplitGetSubKSP( pc, &nblocks, &kspArray);
    KSPSetType( kspArray[0], KSPPREONLY);
    KSPSetType( kspArray[1], KSPPREONLY);
    KSPGetPC(kspArray[0], &pc1);
    KSPGetPC(kspArray[1], &pc2);

    PCSetType( pc1, PCJACOBI);
    PCSetType( pc2, PCGAMG);

    PetscCall( KSPSetFromOptions(ksp) );
    PetscCall( KSPSetUp(ksp) );
    PetscPrintf(PETSC_COMM_WORLD,"Solving the linear system A_hat*X_hat = b_input...\n");

    PetscCall( KSPSolve(ksp,b_input, *X_hat) );

    PCFieldSplitGetType(pc, &pc_composite_type);
    if(pc_composite_type==PC_COMPOSITE_MULTIPLICATIVE)
        PetscPrintf(PETSC_COMM_WORLD,"... linear system solved with ksp_type %s, pc_composite_type PC_COMPOSITE_MULTIPLICATIVE\n",ksp_type);
    else
        PetscPrintf(PETSC_COMM_WORLD,"... linear system solved with ksp_type %s, pc_composite_type %d (different from PC_COMPOSITE_MULTIPLICATIVE)\n",ksp_type,pc_composite_type);

    //Extract informations about the convergence
    KSPConvergedReason reason;
    KSPGetConvergedReason(ksp,&reason);
    KSPGetIterationNumber(ksp,&iter);
    KSPGetResidualNorm( ksp, residu);
    KSPGetTolerances( ksp, &rtol, &abstol, &dtol, &numberMaxOfIter);

    if (reason>0)
        PetscPrintf(PETSC_COMM_WORLD, "Linear system converged in %d iterations \n", iter);
    else
        PetscPrintf(PETSC_COMM_WORLD, "!!!!!!!!!!!!!!!!!! Linear system diverged  after %d iterations !!!!!!!!!!!!!!\n", iter);
        
    PCFieldSplitGetSubKSP( pc, &nblocks, &kspArray);
    KSPGetType( ksp, &ksp_type);
    KSPGetType( kspArray[0], &ksp_type0);
    KSPGetType( kspArray[1], &ksp_type1);
    KSPGetIterationNumber(kspArray[0],&iter1);
    KSPGetIterationNumber(kspArray[1],&iter2);
    KSPGetPC(kspArray[0],&pc1);
    KSPGetPC(kspArray[1],&pc2);
    PCGetType( pc, &pc_type);
    PCGetType( pc1, &pc_type0);
    PCGetType( pc2, &pc_type1);

    PetscPrintf(PETSC_COMM_WORLD, "\n############ : monitoring of the linear solver \n");
    PetscPrintf(PETSC_COMM_WORLD, "Linear solver name: %s, preconditioner %s, %d iterations \n", ksp_type, pc_type, iter);
    PetscPrintf(PETSC_COMM_WORLD, "    sub solver 1 name : %s, preconditioner %s, %d iterations \n", ksp_type0, pc_type0, iter1);
    PetscPrintf(PETSC_COMM_WORLD, "    sub solver 2 name : %s, preconditioner %s, %d iterations \n", ksp_type1, pc_type1, iter2);

    switch(reason){
        case 2:
            PetscPrintf(PETSC_COMM_WORLD, "Residual 2-norm < rtol*||RHS||_2 with rtol = %e, final residual = %e\n\n", rtol, *residu);
            break;
        case 3:
            PetscPrintf(PETSC_COMM_WORLD, "Residual 2-norm < atol with atol = %e, final residual = %e\n\n", abstol, *residu);
            break;
        case -4:
            PetscPrintf(PETSC_COMM_WORLD, "!!!!!!! Residual 2-norm > dtol*||RHS||_2 with dtol = %e, final residual = %e !!!!!!! \n", dtol, *residu);
            break;
        case -3:
            PetscPrintf(PETSC_COMM_WORLD, "!!!!!!! Maximum number of iterations %d reached with dtol = %e, final residual =  %e !!!!!!! \n", numberMaxOfIter, dtol, *residu);
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

    KSPDestroy(&ksp);
    PetscFree(kspArray);

    return PETSC_SUCCESS;
}

//##### Solve the left transformed system for Xoutput
int solveLeftTransformedSystemForXoutput( Mat Ahat, Mat Pmat, IS is_U, IS is_P, Vec b_hat, Vec * X_output, PetscReal rtol, PetscReal abstol, PetscReal dtol, PetscInt numberMaxOfIter, double * residu, PetscBool useLowerTriangularTransform)
{
    KSP ksp, *kspArray;
    PC pc, pc1, pc2;
    KSPType ksp_type0, ksp_type1,  ksp_type = KSPFBCGS;//FBCGS seems much more efficient than FGMRES
    PCType pc_type=PCFIELDSPLIT, pc_type0, pc_type1;
    int nblocks=2, iter, iter1, iter2;//iter = main iteration number, iter1 and iter2 are sub iteration numbers
    PCCompositeType pc_composite_type = PC_COMPOSITE_MULTIPLICATIVE;// MULTIPLICATIVE = block lower triangular preconditioner, ADDITIVE  = block diagonal preconditioner

    PetscPrintf(PETSC_COMM_WORLD,"Setting the solver ...\n");
    PetscCall( KSPCreate(PETSC_COMM_WORLD,&ksp) );
    PetscCall( KSPSetType(ksp, ksp_type) );
    PetscCall( KSPSetOperators(ksp,Ahat,Pmat) );
    PetscCall( KSPSetTolerances(ksp,rtol, abstol, dtol, numberMaxOfIter) );
    KSPGetPC(ksp,&pc);
    PetscPrintf(PETSC_COMM_WORLD,"Setting the preconditioner %s...\n", pc_type);
    PCSetType(pc,pc_type);

    PCFieldSplitSetType(pc, pc_composite_type);
    PCFieldSplitSetIS(pc, "0",is_P);//The order here matters a lot between this line and the next
    PCFieldSplitSetIS(pc, "1",is_U);//The order here matters a lot between this line and the previous
    PCFieldSplitGetSubKSP( pc, &nblocks, &kspArray);
    KSPSetType( kspArray[0], KSPPREONLY);
    KSPSetType( kspArray[1], KSPPREONLY);
    KSPGetPC(kspArray[0], &pc1);
    KSPGetPC(kspArray[1], &pc2);

    if( useLowerTriangularTransform )//Default, faster
    {
    PCSetType( pc1, PCGAMG);
    PCSetType( pc2, PCJACOBI);
    }
    else
    {
    PCSetType( pc2, PCGAMG);
    PCSetType( pc1, PCJACOBI);
    }

    PetscCall( KSPSetFromOptions(ksp) );
    PetscCall( KSPSetUp(ksp) );
    PetscPrintf(PETSC_COMM_WORLD,"Solving the linear system A_hat*X_output = b_hat...\n");

    PetscCall( KSPSolve(ksp,b_hat, *X_output) );

    PCFieldSplitGetType(pc, &pc_composite_type);
    if(pc_composite_type==PC_COMPOSITE_MULTIPLICATIVE)
        PetscPrintf(PETSC_COMM_WORLD,"... linear system solved with ksp_type %s, pc_composite_type PC_COMPOSITE_MULTIPLICATIVE\n",ksp_type);
    else
        PetscPrintf(PETSC_COMM_WORLD,"... linear system solved with ksp_type %s, pc_composite_type %d (different from PC_COMPOSITE_MULTIPLICATIVE)\n",ksp_type,pc_composite_type);

    //Extract informations about the convergence
    KSPConvergedReason reason;
    KSPGetConvergedReason(ksp,&reason);
    KSPGetIterationNumber(ksp,&iter);
    KSPGetResidualNorm( ksp, residu);
    KSPGetTolerances( ksp, &rtol, &abstol, &dtol, &numberMaxOfIter);

    if (reason>0)
        PetscPrintf(PETSC_COMM_WORLD, "Linear system converged in %d iterations \n", iter);
    else
        PetscPrintf(PETSC_COMM_WORLD, "!!!!!!!!!!!!!!!!!! Linear system diverged  after %d iterations !!!!!!!!!!!!!!\n", iter);
        
    PCFieldSplitGetSubKSP( pc, &nblocks, &kspArray);
    KSPGetType( ksp, &ksp_type);
    KSPGetType( kspArray[0], &ksp_type0);
    KSPGetType( kspArray[1], &ksp_type1);
    KSPGetIterationNumber(kspArray[0],&iter1);
    KSPGetIterationNumber(kspArray[1],&iter2);
    KSPGetPC(kspArray[0],&pc1);
    KSPGetPC(kspArray[1],&pc2);
    PCGetType( pc, &pc_type);
    PCGetType( pc1, &pc_type0);
    PCGetType( pc2, &pc_type1);

    PetscPrintf(PETSC_COMM_WORLD, "\n############ : monitoring of the linear solver \n");
    PetscPrintf(PETSC_COMM_WORLD, "Linear solver name: %s, preconditioner %s, %d iterations \n", ksp_type, pc_type, iter);
    PetscPrintf(PETSC_COMM_WORLD, "    sub solver 1 name : %s, preconditioner %s, %d iterations \n", ksp_type0, pc_type0, iter1);
    PetscPrintf(PETSC_COMM_WORLD, "    sub solver 2 name : %s, preconditioner %s, %d iterations \n", ksp_type1, pc_type1, iter2);

    switch(reason){
        case 2:
            PetscPrintf(PETSC_COMM_WORLD, "Residual 2-norm < rtol*||RHS||_2 with rtol = %e, final residual = %e\n\n", rtol, *residu);
            break;
        case 3:
            PetscPrintf(PETSC_COMM_WORLD, "Residual 2-norm < atol with atol = %e, final residual = %e\n\n", abstol, *residu);
            break;
        case -4:
            PetscPrintf(PETSC_COMM_WORLD, "!!!!!!! Residual 2-norm > dtol*||RHS||_2 with dtol = %e, final residual = %e !!!!!!! \n", dtol, *residu);
            break;
        case -3:
            PetscPrintf(PETSC_COMM_WORLD, "!!!!!!! Maximum number of iterations %d reached with dtol = %e, final residual =  %e !!!!!!! \n", numberMaxOfIter, dtol, *residu);
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

    KSPDestroy(&ksp);
    PetscFree(kspArray);

    return PETSC_SUCCESS;
}

//##### Application of the transformation A -> A_hat by multiplication to the right by an upper triangular matrix and to the left by a lower triangular matrix
//## Vector v must be deleted by caller
/*                                 *M   G*                                                       */
/*                        A     = *       *                                                      */
/*                                 *D   C*                                                       */
/*                                                                                               */
/*                                 *Id  -diag(M)^{-1}G*                    *Id  diag(M)^{-1}G*   */
/*                        U     = *                    *         U^{-1} = *                   *  */
/*                                 *0               Id*                    *0              Id*   */
/*                                                                                               */
/*                                 *Id               0*                    *Id              0*   */
/*                        L     = *                    *         L^{-1} = *                   *  */
/*                                 *-D*diag(M)^{-1} Id*                    *D*diag(M)^{-1} Id*   */
/*                                                                                               */
/*                                 *M     G_hat*             G_hat=G - M*diag(M)^{-1}*G   D_hat=D - D*diag(M)^{-1}*M         */
/*                        A_hat = *             *                                                                            */
/*                                 *Dhat  C_hat2*            C_hat=C - D*diag(M)^{-1}*G   C_hat2=Chat - D*diag(M)^{-1}*Ghat  */
/*                                                                                               */
/*                                 *2 diag(M)     0  *                                           */
/*                        Pmat  = *                   *                                          */
/*                                 *Dhat          C_hat2 *                                       */
/*                                                                                               */
/*************************************************************************************************/
int transformSystemLeftRight( Mat M, Mat G, Mat D, Mat C, Mat * A_hat, Mat * Pmat, Mat * C_hat, Mat * G_hat, Mat * D_hat, Mat * diag_2M, Vec * v)
{
    PetscPrintf(PETSC_COMM_WORLD,"Transformation of the original system matrix by multiplication to the right...\n");

    Vec v_redistributed;
    Mat D_M_inv_G, D_DM_inv, *C_hat2, Mat_array[4];// D_M_inv = diag(M)^{-1}, D_DM_inv = D*diag(M)^{-1}
    VecScatter scat;//tool to redistribute a vector on the processors
    IS is_to, is_from;
    PetscInt col_min, col_max;

    //Extraction of the diagonal of M
    MatCreateVecs(M,NULL,v);//v has the size of M
    MatGetDiagonal(M,*v);

    //Creation of matrix 2*diag(M). Why not use MatCreateDiagonal ???
    PetscCall( MatDuplicate(M, MAT_DO_NOT_COPY_VALUES, diag_2M) );
    MatEliminateZeros(*diag_2M, PETSC_TRUE);
    MatDiagonalSet(*diag_2M, *v,  INSERT_VALUES);
    MatScale(*diag_2M,2);//store 2*diagonal part of M
    PetscCall( VecReciprocal(*v) );//Must first check that all the coefficients are non zero
    
    // Creation of D_M_inv_G = D_M_inv*G = diag(M)^{-1} * G
    PetscCall( MatDuplicate(G,MAT_COPY_VALUES,&D_M_inv_G) );//D_M_inv_G contains G
    PetscCall( MatDuplicate(D,MAT_COPY_VALUES,&D_DM_inv) );//D_DM_inv contains D
    PetscCall( MatCreateVecs(D_M_inv_G,NULL,&v_redistributed) );//v_redistributed has the parallel distribution of D_M_inv_G
    VecGetOwnershipRange(*v,&col_min,&col_max);
    ISCreateStride(PETSC_COMM_WORLD, col_max-col_min, col_min, 1, &is_from);
    VecGetOwnershipRange(v_redistributed,&col_min,&col_max);
    ISCreateStride(PETSC_COMM_WORLD, col_max-col_min, col_min, 1, &is_to);
    VecScatterCreate(*v,is_from,v_redistributed,is_to,&scat);
    VecScatterBegin(scat, *v, v_redistributed,INSERT_VALUES,SCATTER_FORWARD);
    VecScatterEnd(  scat, *v, v_redistributed,INSERT_VALUES,SCATTER_FORWARD);
    MatDiagonalScale( D_M_inv_G, v_redistributed, NULL);//D_M_inv_G contains D_M_inv*G
    MatDiagonalScale( D_DM_inv, NULL, v_redistributed);//D_DM_inv contains D_DM_inv

    // Creation of C_hat
    MatMatMult(D,D_M_inv_G,MAT_INITIAL_MATRIX,PETSC_DEFAULT,C_hat);//C_hat contains D*D_M_inv*G
    MatAYPX(*C_hat,-1.0,C,SUBSET_NONZERO_PATTERN);//C_hat contains C - D*D_M_inv*G

    // Creation of G_hat
    MatMatMult(M,D_M_inv_G,MAT_INITIAL_MATRIX,PETSC_DEFAULT,G_hat);//G_hat contains M*D_M_inv*G
    MatAYPX(*G_hat,-1.0,G,UNKNOWN_NONZERO_PATTERN);//G_hat contains G - M*D_M_inv*G

    // Creation of D_hat
    MatMatMult(D_DM_inv,M,MAT_INITIAL_MATRIX,PETSC_DEFAULT,D_hat);//D_hat contains D*DM_inv*M
    MatAYPX(*D_hat,-1.0,D,UNKNOWN_NONZERO_PATTERN);//D_hat contains D - D*DM_inv*M

    // Creation of C_hat2
    MatMatMult(D_DM_inv, *G_hat,MAT_INITIAL_MATRIX,PETSC_DEFAULT,C_hat2);//C_hat2 contains D*D_M_inv*Ghat
    MatAYPX(*C_hat2,-1.0,*C_hat,SUBSET_NONZERO_PATTERN);//C_hat2 contains Chat - D*D_M_inv*Ghat

    //Creation of global matrices using MatCreateNest
    Mat_array[3]=*C_hat2;//Top left block of A_hat
    Mat_array[2]=*D_hat;//Top right block of A_hat
    Mat_array[1]=*G_hat;//Bottom left block of A_hat
    Mat_array[0]=M;//Bottom left block of A_hat

    // Creation of A_hat = reordered A_input
    MatCreateNest(PETSC_COMM_WORLD,2,NULL,2,NULL,Mat_array,A_hat);

    // Creation of Pmat
    Mat_array[0]=*diag_2M;//Replace M by its diagonal to ease inversion
    MatCreateNest(PETSC_COMM_WORLD,2,NULL,2,NULL,Mat_array,Pmat);

    PetscPrintf(PETSC_COMM_WORLD,"... matrix transformed \n");    

    MatDestroy(diag_2M);
    MatDestroy(C_hat);
    MatDestroy(C_hat2);
    MatDestroy(G_hat);
    MatDestroy(D_hat);
    MatDestroy(&D_M_inv_G);
    VecScatterDestroy(&scat);
    VecDestroy(&v_redistributed);
}

//##### Solve the left-right transformed system for Xhat
int solveLeftRightTransformedSystemForXhat( Mat A_hat, Mat Pmat, IS is_U, IS is_P, Vec b_hat, Vec * X_hat, PetscReal rtol, PetscReal abstol, PetscReal dtol, PetscInt numberMaxOfIter, double * residu)
{
    KSP ksp, *kspArray;
    PC pc, pc1, pc2;
    KSPType ksp_type0, ksp_type1,  ksp_type = KSPFBCGS;//BCGS seems very efficient
    PCType pc_type=PCFIELDSPLIT, pc_type0, pc_type1;
    int nblocks=2, iter, iter1, iter2;//iter = main iteration number, iter1 and iter2 are sub iteration numbers
    PCCompositeType pc_composite_type = PC_COMPOSITE_MULTIPLICATIVE;// MULTIPLICATIVE = block triangular preconditioner, ADDITIVE  = block diagonal preconditioner

    PetscPrintf(PETSC_COMM_WORLD,"Setting the solver ...\n");
    KSPCreate(PETSC_COMM_WORLD,&ksp);
    KSPSetType(ksp, ksp_type);
    PetscCall( KSPSetOperators(ksp,A_hat,Pmat) );
    KSPSetTolerances(ksp,rtol, abstol, dtol, numberMaxOfIter);
    KSPGetPC(ksp,&pc);
    PetscPrintf(PETSC_COMM_WORLD,"Setting the preconditioner %s...\n", pc_type);
    PCSetType(pc,pc_type);

    PCFieldSplitSetType(pc, pc_composite_type);
    PCFieldSplitSetIS(pc, "0",is_U);//The order here matters a lot between this line and the next
    PCFieldSplitSetIS(pc, "1",is_P);//The order here matters a lot between this line and the previous
    PCFieldSplitGetSubKSP( pc, &nblocks, &kspArray);
    KSPSetType( kspArray[0], KSPPREONLY);
    KSPSetType( kspArray[1], KSPPREONLY);
    KSPGetPC(kspArray[0], &pc1);
    KSPGetPC(kspArray[1], &pc2);

    PCSetType( pc1, PCJACOBI);
    PCSetType( pc2, PCGAMG);

    PetscCall( KSPSetFromOptions(ksp) );
    PetscCall( KSPSetUp(ksp) );
    PetscPrintf(PETSC_COMM_WORLD,"Solving the linear system A_hat*X_hat = b_hat ...\n");

    PetscCall( KSPSolve(ksp,b_hat, *X_hat) );

    PCFieldSplitGetType(pc, &pc_composite_type);
    if(pc_composite_type==PC_COMPOSITE_MULTIPLICATIVE)
        PetscPrintf(PETSC_COMM_WORLD,"... linear system solved with ksp_type %s, pc_composite_type PC_COMPOSITE_MULTIPLICATIVE\n",ksp_type);
    else
        PetscPrintf(PETSC_COMM_WORLD,"... linear system solved with ksp_type %s, pc_composite_type %d (different from PC_COMPOSITE_MULTIPLICATIVE)\n",ksp_type,pc_composite_type);

    //Extract informations about the convergence
    KSPConvergedReason reason;
    KSPGetConvergedReason(ksp,&reason);
    KSPGetIterationNumber(ksp,&iter);
    KSPGetResidualNorm( ksp, residu);
    KSPGetTolerances( ksp, &rtol, &abstol, &dtol, &numberMaxOfIter);

    if (reason>0)
        PetscPrintf(PETSC_COMM_WORLD, "Linear system converged in %d iterations \n", iter);
    else
        PetscPrintf(PETSC_COMM_WORLD, "!!!!!!!!!!!!!!!!!! Linear system diverged  after %d iterations !!!!!!!!!!!!!!\n", iter);
        
    PCFieldSplitGetSubKSP( pc, &nblocks, &kspArray);
    KSPGetType( ksp, &ksp_type);
    KSPGetType( kspArray[0], &ksp_type0);
    KSPGetType( kspArray[1], &ksp_type1);
    KSPGetIterationNumber(kspArray[0],&iter1);
    KSPGetIterationNumber(kspArray[1],&iter2);
    KSPGetPC(kspArray[0],&pc1);
    KSPGetPC(kspArray[1],&pc2);
    PCGetType( pc, &pc_type);
    PCGetType( pc1, &pc_type0);
    PCGetType( pc2, &pc_type1);

    PetscPrintf(PETSC_COMM_WORLD, "\n############ : monitoring of the linear solver \n");
    PetscPrintf(PETSC_COMM_WORLD, "Linear solver name: %s, preconditioner %s, %d iterations \n", ksp_type, pc_type, iter);
    PetscPrintf(PETSC_COMM_WORLD, "    sub solver 1 name : %s, preconditioner %s, %d iterations \n", ksp_type0, pc_type0, iter1);
    PetscPrintf(PETSC_COMM_WORLD, "    sub solver 2 name : %s, preconditioner %s, %d iterations \n", ksp_type1, pc_type1, iter2);

    switch(reason){
        case 2:
            PetscPrintf(PETSC_COMM_WORLD, "Residual 2-norm < rtol*||RHS||_2 with rtol = %e, final residual = %e\n\n", rtol, *residu);
            break;
        case 3:
            PetscPrintf(PETSC_COMM_WORLD, "Residual 2-norm < atol with atol = %e, final residual = %e\n\n", abstol, *residu);
            break;
        case -4:
            PetscPrintf(PETSC_COMM_WORLD, "!!!!!!! Residual 2-norm > dtol*||RHS||_2 with dtol = %e, final residual = %e !!!!!!! \n", dtol, *residu);
            break;
        case -3:
            PetscPrintf(PETSC_COMM_WORLD, "!!!!!!! Maximum number of iterations %d reached with dtol = %e, final residual =  %e !!!!!!! \n", numberMaxOfIter, dtol, *residu);
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

    KSPDestroy(&ksp);
    PetscFree(kspArray);

    return PETSC_SUCCESS;
}

