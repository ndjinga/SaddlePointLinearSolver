#include "SaddlePointLinearSolver.h"

/*
    User-defined application context
 */
typedef struct {
  IS          is_U;  /* indices of velocity lines */
  IS          is_P;  /* indices of pressure lines */
  PC          pcM;   /* preconditioner containing the ILU decomposition of the top left matrix M */
  Mat         G;     /* top right submatrix */
  Vec         X_p;//Pressure components of the transformed unknown
  Vec         X_u;//Velocity components of the transformed unknown
  Vec         GX_p;//Storage of G*X_p
  Vec         Y_u;//Storage of M^{-1}*G*X_p
  Vec         Y_p;
} ApplicationCtx2x2;

/* setup function for the right preconditioner */
PetscErrorCode setupRight(PC pcshell)
{
    ApplicationCtx2x2 * ctx;
    
    PetscFunctionBegin;
    PetscCall(PCShellGetContext( pcshell, &ctx));
    MatCreateVecs( ctx->G, NULL, &ctx->GX_p );
    PetscFunctionReturn(PETSC_SUCCESS);
}

/* destroy function for the right preconditioner */
PetscErrorCode destroyRight(PC pcshell)
{
    ApplicationCtx2x2 * ctx;
    
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

/* multiplication operator for the right preconditioner */
PetscErrorCode applyRight(PC pcshell, Vec x, Vec y)
{
    ApplicationCtx2x2 * ctx;
    
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
int transformSystemRight( Mat M, Mat G, Mat D, Mat C, Mat * A_hat, Mat * Pmat, Vec * v)
{
    PetscPrintf(PETSC_COMM_WORLD,"Transformation of the original system matrix by multiplication to the right by an upper triangular matrix U : Ahat = A_input*U ...\n");

    Vec v_redistributed;
    Mat D_M_inv_G, Mat_array[4];// D_M_inv = diag(M)^{-1}
    Mat C_hat, G_hat, diag_2M;
    VecScatter scat;//tool to redistribute a vector on the processors
    IS is_to, is_from;
    PetscInt col_min, col_max;

    //Extraction of the diagonal of M
    MatCreateVecs(M,NULL,v);//v has the size of M
    MatGetDiagonal(M,*v);

    //Creation of matrix 2*diag(M). Why not use MatCreateDiagonal ???
    PetscCall( MatDuplicate(M, MAT_DO_NOT_COPY_VALUES, &diag_2M) );
    MatEliminateZeros(diag_2M, PETSC_TRUE);
    MatDiagonalSet(diag_2M, *v,  INSERT_VALUES);
    MatScale(diag_2M,2);//store 2*diagonal part of M
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
    MatMatMult(D,D_M_inv_G,MAT_INITIAL_MATRIX,PETSC_DEFAULT,&C_hat);//C_hat contains D*D_M_inv*G
    MatAYPX(C_hat,-1.0,C,SUBSET_NONZERO_PATTERN);//C_hat contains C - D*D_M_inv*G

    // Creation of G_hat
    MatMatMult(M,D_M_inv_G,MAT_INITIAL_MATRIX,PETSC_DEFAULT,&G_hat);//G_hat contains M*D_M_inv*G
    MatAYPX(G_hat,-1.0,G,UNKNOWN_NONZERO_PATTERN);//G_hat contains G - M*D_M_inv*G

    //Creation of global matrices using MatCreateNest
    Mat_array[3]=C_hat;//Top right block of A_hat
    Mat_array[2]=D;//Bottom left block of A_hat
    Mat_array[1]=G_hat;//Top right block of A_hat
    Mat_array[0]=M;//Top left block of A_hat

    // Creation of A_hat 
    MatCreateNest(PETSC_COMM_WORLD,2,NULL,2,NULL,Mat_array,A_hat);

    // Creation of Pmat
    Mat_array[0]=diag_2M;//Replace M by its diagonal to ease inversion
    MatCreateNest(PETSC_COMM_WORLD,2,NULL,2,NULL,Mat_array,Pmat);

    PetscPrintf(PETSC_COMM_WORLD,"... matrix transformed \n");    

    MatDestroy(&diag_2M);
    MatDestroy(&C_hat);
    MatDestroy(&G_hat);
    MatDestroy(&D_M_inv_G);
    VecScatterDestroy(&scat);
    VecDestroy(&v_redistributed);
}

int getAhatRight( Mat M, Mat G, Mat D, Mat C, Mat * A_hat )
{
    PetscPrintf(PETSC_COMM_WORLD,"Construction of the transform matrix by multiplication to the right by an upper triangular matrix U : Ahat = A_input*U ...\n");

    Vec v, v_redistributed;
    Mat D_M_inv_G, Mat_array[4];// D_M_inv = diag(M)^{-1}
    Mat C_hat, G_hat, diag_2M;
    VecScatter scat;//tool to redistribute a vector on the processors
    IS is_to, is_from;
    PetscInt col_min, col_max;

    //Extraction of the diagonal of M
    PetscCall( MatCreateVecs(M,NULL,&v) );//v has the size of M
    PetscCall( MatGetDiagonal(M,v) );

    //Creation of matrix 2*diag(M). Why not use MatCreateDiagonal ???
    PetscCall( MatDuplicate(M, MAT_DO_NOT_COPY_VALUES, &diag_2M) );
    MatEliminateZeros(diag_2M, PETSC_TRUE);
    MatDiagonalSet(diag_2M, v,  INSERT_VALUES);
    MatScale(diag_2M,2);//store 2*diagonal part of M
    PetscCall( VecReciprocal(v) );//Must first check that all the coefficients are non zero
    
    // Creation of D_M_inv_G = D_M_inv*G = diag(M)^{-1} * G
    PetscCall( MatDuplicate(G,MAT_COPY_VALUES,&D_M_inv_G) );//D_M_inv_G contains G
    PetscCall( MatCreateVecs(D_M_inv_G,NULL,&v_redistributed) );//v_redistributed has the parallel distribution of D_M_inv_G
    VecGetOwnershipRange(v,&col_min,&col_max);
    ISCreateStride(PETSC_COMM_WORLD, col_max-col_min, col_min, 1, &is_from);
    VecGetOwnershipRange(v_redistributed,&col_min,&col_max);
    ISCreateStride(PETSC_COMM_WORLD, col_max-col_min, col_min, 1, &is_to);
    VecScatterCreate(v,is_from,v_redistributed,is_to,&scat);
    VecScatterBegin(scat, v, v_redistributed,INSERT_VALUES,SCATTER_FORWARD);
    VecScatterEnd(  scat, v, v_redistributed,INSERT_VALUES,SCATTER_FORWARD);
    MatDiagonalScale( D_M_inv_G, v_redistributed, NULL);//D_M_inv_G contains D_M_inv*G

    // Creation of C_hat
    MatMatMult(D,D_M_inv_G,MAT_INITIAL_MATRIX,PETSC_DEFAULT,&C_hat);//C_hat contains D*D_M_inv*G
    MatAYPX(C_hat,-1.0,C,SUBSET_NONZERO_PATTERN);//C_hat contains C - D*D_M_inv*G

    // Creation of G_hat
    MatMatMult(M,D_M_inv_G,MAT_INITIAL_MATRIX,PETSC_DEFAULT,&G_hat);//G_hat contains M*D_M_inv*G
    MatAYPX(G_hat,-1.0,G,UNKNOWN_NONZERO_PATTERN);//G_hat contains G - M*D_M_inv*G

    //Creation of global matrices using MatCreateNest
    Mat_array[3]=C_hat;//Top right block of A_hat
    Mat_array[2]=D;//Bottom left block of A_hat
    Mat_array[1]=G_hat;//Top right block of A_hat
    Mat_array[0]=M;//Top left block of A_hat

    // Creation of A_hat 
    MatCreateNest(PETSC_COMM_WORLD,2,NULL,2,NULL,Mat_array,A_hat);

    PetscPrintf(PETSC_COMM_WORLD,"... matrix transformed \n");    

    MatDestroy(&diag_2M);
    MatDestroy(&C_hat);
    MatDestroy(&G_hat);
    MatDestroy(&D_M_inv_G);
    VecScatterDestroy(&scat);
    VecDestroy(&v);
    VecDestroy(&v_redistributed);
}

int getChatRight( Mat M, Mat G, Mat D, Mat C, Mat * C_hat )
{
    PetscPrintf(PETSC_COMM_WORLD,"Computing the Schur complement C_hat of A_input ...\n");

    Vec v, v_redistributed;
    Mat D_M_inv_G;// D_M_inv = diag(M)^{-1}
    Mat  diag_2M;
    VecScatter scat;//tool to redistribute a vector on the processors
    IS is_to, is_from;
    PetscInt col_min, col_max;

    //Extraction of the diagonal of M
    PetscCall( MatCreateVecs(M,NULL,&v) );//v has the size of M
    PetscCall( MatGetDiagonal(M,v) );

    //Creation of matrix 2*diag(M). Why not use MatCreateDiagonal ???
    PetscCall( MatDuplicate(M, MAT_DO_NOT_COPY_VALUES, &diag_2M) );
    MatEliminateZeros(diag_2M, PETSC_TRUE);
    MatDiagonalSet(diag_2M, v,  INSERT_VALUES);
    MatScale(diag_2M,2);//store 2*diagonal part of M
    PetscCall( VecReciprocal(v) );//Must first check that all the coefficients are non zero
    
    // Creation of D_M_inv_G = D_M_inv*G = diag(M)^{-1} * G
    PetscCall( MatDuplicate(G,MAT_COPY_VALUES,&D_M_inv_G) );//D_M_inv_G contains G
    PetscCall( MatCreateVecs(D_M_inv_G,NULL,&v_redistributed) );//v_redistributed has the parallel distribution of D_M_inv_G
    VecGetOwnershipRange(v,&col_min,&col_max);
    ISCreateStride(PETSC_COMM_WORLD, col_max-col_min, col_min, 1, &is_from);
    VecGetOwnershipRange(v_redistributed,&col_min,&col_max);
    ISCreateStride(PETSC_COMM_WORLD, col_max-col_min, col_min, 1, &is_to);
    VecScatterCreate(v,is_from,v_redistributed,is_to,&scat);
    VecScatterBegin(scat, v, v_redistributed,INSERT_VALUES,SCATTER_FORWARD);
    VecScatterEnd(  scat, v, v_redistributed,INSERT_VALUES,SCATTER_FORWARD);
    MatDiagonalScale( D_M_inv_G, v_redistributed, NULL);//D_M_inv_G contains D_M_inv*G

    // Creation of C_hat
    MatMatMult(D,D_M_inv_G,MAT_INITIAL_MATRIX,PETSC_DEFAULT,C_hat);//C_hat contains D*D_M_inv*G
    MatAYPX(*C_hat,-1.0,C,SUBSET_NONZERO_PATTERN);//C_hat contains C - D*D_M_inv*G

    MatDestroy(&diag_2M);
    MatDestroy(&D_M_inv_G);
    VecScatterDestroy(&scat);
    VecDestroy(&v);
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
int transformSystemLeft( Mat M, Mat G, Mat D, Mat C, Mat * A_hat, Mat * Pmat, Vec * v, PetscBool useLowerTriangularTransform)
{
    PetscPrintf(PETSC_COMM_WORLD,"Transformation of the original system matrix A_input by multiplication to the left by a lower triangular matrix L : Ahat = L*A_input ...\n");

    Vec v_redistributed;
    Mat D_DM_inv, Mat_array[4];// D_DM_inv = D*diag(M)^{-1}
    Mat C_hat, D_hat, diag_2M;
    VecScatter scat;//tool to redistribute a vector on the processors
    IS is_to, is_from;
    PetscInt col_min, col_max;

    //Extraction of the diagonal of M
    MatCreateVecs(M,NULL,v);//v has the size of M
    MatGetDiagonal(M,*v);

    //Creation of matrix 2*diag(M). Why not use MatCreateDiagonal ???
    PetscCall( MatDuplicate(M, MAT_DO_NOT_COPY_VALUES, &diag_2M) );
    MatEliminateZeros(diag_2M, PETSC_TRUE);
    MatDiagonalSet(diag_2M, *v,  INSERT_VALUES);
    MatScale(diag_2M,2);//store 2*diagonal part of M
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
    MatMatMult(D_DM_inv,G,MAT_INITIAL_MATRIX,PETSC_DEFAULT,&C_hat);//C_hat contains D*D_M_inv*G
    MatAYPX(C_hat,-1.0,C,SUBSET_NONZERO_PATTERN);//C_hat contains C - D*D_M_inv*G

    // Creation of D_hat
    MatMatMult(D_DM_inv,M,MAT_INITIAL_MATRIX,PETSC_DEFAULT,&D_hat);//D_hat contains D*DM_inv*M
    MatAYPX(D_hat,-1.0,D,UNKNOWN_NONZERO_PATTERN);//D_hat contains D - D*DM_inv*M

    //Creation of global matrices using MatCreateNest
    if( useLowerTriangularTransform )//Default, faster
    {
        Mat_array[0]=C_hat;//Top left block of A_hat
        Mat_array[1]=D_hat;//Top right block of A_hat
        Mat_array[3]=M;//Bottom right block of A_hat
        Mat_array[2]=G;//Bottom left block of A_hat
    }
    else
    {
        Mat_array[3]=C_hat;//Bottom right block of A_hat
        Mat_array[2]=D_hat;//Bottom left block of A_hat
        Mat_array[0]=M;//Top left block of A_hat
        Mat_array[1]=G;//Top right block of A_hat
    }

    // Creation of A_hat
    MatCreateNest(PETSC_COMM_WORLD,2,NULL,2,NULL,Mat_array,A_hat);

    // Creation of Pmat
    if( useLowerTriangularTransform )
        Mat_array[3]=diag_2M;//Replace M by its diagonal to ease inversion
    else
        Mat_array[0]=diag_2M;//Replace M by its diagonal to ease inversion
    MatCreateNest(PETSC_COMM_WORLD,2,NULL,2,NULL,Mat_array,Pmat);

    PetscPrintf(PETSC_COMM_WORLD,"... matrix transformed \n");    

    MatDestroy(&diag_2M);
    MatDestroy(&C_hat);
    MatDestroy(&D_hat);
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

    VecRestoreSubVector( X_hat, is_P, &X_hat_p);
    VecRestoreSubVector( X_hat, is_U, &X_hat_u);
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

    VecRestoreSubVector( X_anal, is_P, &X_anal_p);
    VecRestoreSubVector( X_anal, is_U, &X_anal_u);

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
    PetscCall( VecGetSubVector( b_input, is_P, &b_hat_p_tmp) );
    PetscCall( VecGetSubVector( b_input, is_U, &b_hat_u_tmp) );

    PetscCall( VecDuplicate(b_hat_u_tmp,&b_hat_u) );
    PetscCall( VecDuplicate(b_hat_p_tmp,&b_hat_p) );
    VecCopy(b_hat_u_tmp,b_hat_u);
    PetscCall( VecPointwiseMult(b_hat_u_tmp,b_hat_u_tmp,v) );

    PetscCall( MatMult( D, b_hat_u_tmp, b_hat_p) );
    PetscCall( VecAYPX( b_hat_p, -1, b_hat_p_tmp) );

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

    PetscCall( VecRestoreSubVector( b_input, is_P, &b_hat_p_tmp) );
    PetscCall( VecRestoreSubVector( b_input, is_U, &b_hat_u_tmp) );
}

//##### Solve the right transformed system for Xhat
int solveRightTransformedSystemForXhat( Mat A_hat, Mat Pmat, IS is_U, IS is_P, Vec b_input, Vec * X_hat, PetscReal rtol, PetscReal abstol, PetscReal dtol, PetscInt numberMaxOfIter, double *residu)
{
    KSP ksp, *kspArray;
    PC pc, pc1, pc2;
    KSPType ksp_type1,  ksp_type = KSPFBCGS;//BCGS seems very efficient
    PCType pc_type=PCFIELDSPLIT;
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
    PetscCall( PCSetUp( pc) );
    PCFieldSplitGetSubKSP( pc, NULL, &kspArray);
    KSPSetType( kspArray[0], KSPPREONLY);
    KSPSetType( kspArray[1], KSPPREONLY);
    KSPGetPC(kspArray[0], &pc1);
    KSPGetPC(kspArray[1], &pc2);

    PCSetType( pc1, PCJACOBI);
    PCSetType( pc2, PCGAMG);

    PetscCall( KSPSetFromOptions(ksp) );
    PetscCall( KSPSetUp(ksp) );
    PetscPrintf(PETSC_COMM_WORLD,"Solving the linear system A_hat*X_hat = b_input with a fieldsplit preconditioner...\n");

    PetscCall( KSPSolve(ksp,b_input, *X_hat) );

    //Extract and display informations about the convergence
    displayPCFieldSplitIterationNumbers( &ksp, residu);
    
    KSPDestroy(&ksp);
    PetscFree(kspArray);

    return PETSC_SUCCESS;
}

//##### Solve the right transformed system for Xhat
int solveRightILUTransformedSystemForXoutput( Mat A_input, Mat A_hat, Mat M, Mat G, IS is_U, IS is_P, Vec b_input, Vec * X_output, PetscReal rtol, PetscReal abstol, PetscReal dtol, PetscInt numberMaxOfIter, double *residu)
{
    KSP ksp;
    KSPType ksp_type = KSPFBCGS;//BCGS seems very efficient
    PC pc;

    PetscPrintf(PETSC_COMM_WORLD,"Setting the main solver ...\n");
    KSPCreate(PETSC_COMM_WORLD,&ksp);
    KSPSetType(ksp, ksp_type);
    PetscCall( KSPSetOperators(ksp,A_input,A_hat) );
    KSPSetTolerances(ksp,rtol, abstol, dtol, numberMaxOfIter);
    KSPGetPC(ksp,&pc);
    PetscPrintf(PETSC_COMM_WORLD,"Setting the preconditioner ...\n");
    PCSetType(pc,PCCOMPOSITE);
    PCCompositeSetType( pc, PC_COMPOSITE_MULTIPLICATIVE);

//#### The PCFIELDSPLIT preconditioner (based on GAMG and ILU) ###//
    KSP *kspArray;
    PC pcfieldsplit, pcfieldsplit1, pcfieldsplit2;

    PCCreate(PETSC_COMM_WORLD,&pcfieldsplit);
    PCSetType(pcfieldsplit,PCFIELDSPLIT);
    PetscCall( PCSetOperators(pcfieldsplit,A_input,A_hat) );
    PCFieldSplitSetType(pcfieldsplit, PC_COMPOSITE_MULTIPLICATIVE);// MULTIPLICATIVE = block triangular preconditioner, ADDITIVE  = block diagonal preconditioner
    PCFieldSplitSetIS(pcfieldsplit, "0",is_U);//The order here matters a lot between this line and the next
    PCFieldSplitSetIS(pcfieldsplit, "1",is_P);//The order here matters a lot between this line and the previous
    PetscCall( PCSetUp( pcfieldsplit) );
    PCFieldSplitGetSubKSP( pcfieldsplit, NULL, &kspArray);
    KSPSetType( kspArray[0], KSPPREONLY);
    KSPSetType( kspArray[1], KSPPREONLY);
    KSPGetPC(kspArray[0], &pcfieldsplit1);
    KSPGetPC(kspArray[1], &pcfieldsplit2);

    PCSetType( pcfieldsplit1, PCBJACOBI);
    PCSetType( pcfieldsplit2, PCGAMG);

//### The upper triangular preconditioner corresponding to the triangular transform ####
    PC pctransform;
    ApplicationCtx2x2 ctx = 
    {
      .is_U = is_U,              /* indices of velocity lines */
      .is_P = is_P,              /* indices of pressure lines */
       .pcM = pcfieldsplit1,     /* ILU factorisation of the top left submatrix M */
         .G = G                  /* top right submatrix */
    };

    PCCreate(PETSC_COMM_WORLD,&pctransform);
    PCSetType(pctransform,PCSHELL);
    PCShellSetContext(pctransform,&ctx);
    PCShellSetApply(pctransform,applyRight);
    PCShellSetSetUp(pctransform,setupRight);                   
    PCShellSetDestroy(pctransform,destroyRight);               

//#### Setting the KSP solver ###//
    PCCompositeAddPC( pc, pcfieldsplit);
    PCCompositeAddPC( pc, pctransform);
    PetscCall( KSPSetFromOptions(ksp) );
    PetscCall( KSPSetUp(ksp) );
    PetscPrintf(PETSC_COMM_WORLD,"Solving the linear system A_hat*X_output = b_input with a composite preconditioner...\n");

    PetscCall( KSPSolve(ksp,b_input, *X_output) );

    //Extract and display informations about the convergence
    displayPCCompositeIterationNumbers( &ksp, residu);
    
    KSPDestroy(&ksp);
    PetscFree(kspArray);

    return PETSC_SUCCESS;
}

//##### Use Schur complement factorisation to solve the system for Xoutput
int solveSchurSystemForXoutput( Mat A_input, IS is_U, IS is_P, Vec b_input, Vec * X_output, PetscReal rtol, PetscReal abstol, PetscReal dtol, PetscInt numberMaxOfIter, double *residu)
{
    KSP ksp;
    KSPType ksp_type = KSPFBCGS;//BCGS seems very efficient
    PC pc;

    PetscPrintf(PETSC_COMM_WORLD,"Setting the main solver ...\n");
    KSPCreate(PETSC_COMM_WORLD,&ksp);
    KSPSetType(ksp, ksp_type);
    PetscCall( KSPSetOperators(ksp,A_input,A_input) );
    KSPSetTolerances(ksp,rtol, abstol, dtol, numberMaxOfIter);
    KSPGetPC(ksp,&pc);
    PetscPrintf(PETSC_COMM_WORLD,"Setting the preconditioner ...\n");
    PCSetType(pc,PCFIELDSPLIT);
    PCFieldSplitSetType( pc, PC_COMPOSITE_SCHUR);

//#### The PCFIELDSPLIT preconditioner (based on GAMG and ILU) ###//
    KSP *kspArray;
    PC pcfieldsplit1, pcfieldsplit2;

    PCFieldSplitSetIS(pc, "0",is_U);//The order here matters a lot between this line and the next
    PCFieldSplitSetIS(pc, "1",is_P);//The order here matters a lot between this line and the previous
    PCFieldSplitSetSchurPre(pc,PC_FIELDSPLIT_SCHUR_PRE_SELFP,NULL);//or PC_FIELDSPLIT_SCHUR_PRE_USER if you provide Chat
    PCFieldSplitSetSchurFactType( pc, PC_FIELDSPLIT_SCHUR_FACT_FULL);
    PetscCall( PCSetUp( pc) );
    PCFieldSplitSchurGetSubKSP( pc, NULL, &kspArray);
    KSPSetType( kspArray[0], KSPPREONLY);
    KSPSetType( kspArray[1], KSPPREONLY);
    KSPGetPC(kspArray[0], &pcfieldsplit1);
    KSPGetPC(kspArray[1], &pcfieldsplit2);

    PCSetType( pcfieldsplit1, PCJACOBI);
    PCSetType( pcfieldsplit2, PCBJACOBI);

    PetscCall( KSPSetFromOptions(ksp) );
    PetscCall( KSPSetUp(ksp) );
    PetscPrintf(PETSC_COMM_WORLD,"Solving the linear system A_input*X_output = b_input with a Schur preconditioner...\n");

    PetscCall( KSPSolve(ksp,b_input, *X_output) );

    //Extract and display informations about the convergence
    displayPCFieldSplitIterationNumbers( &ksp, residu);
    
    KSPDestroy(&ksp);
    PetscFree(kspArray);

    return PETSC_SUCCESS;
}

//##### Use Schur complement factorisation with Hypre boomeramg to solve the system for Xoutput
int solveSchurHypreSystemForXoutput( Mat A_input, IS is_U, IS is_P, Vec b_input, Vec * X_output, PetscReal rtol, PetscReal abstol, PetscReal dtol, PetscInt numberMaxOfIter, double *residu)
{
    KSP ksp;
    KSPType ksp_type = KSPFBCGS;//BCGS seems very efficient
    PC pc;
    int iter;

    PetscPrintf(PETSC_COMM_WORLD,"Setting the main solver ...\n");
    KSPCreate(PETSC_COMM_WORLD,&ksp);
    KSPSetType(ksp, ksp_type);
    PetscCall( KSPSetOperators(ksp,A_input,A_input) );
    KSPSetTolerances(ksp,rtol, abstol, dtol, numberMaxOfIter);
    KSPGetPC(ksp,&pc);
    PetscPrintf(PETSC_COMM_WORLD,"Setting the preconditioner ...\n");
    PCSetType(pc,PCFIELDSPLIT);
    PCFieldSplitSetType( pc, PC_COMPOSITE_SCHUR);

//#### The PCFIELDSPLIT preconditioner (based on GAMG and ILU) ###//
    KSP *kspArray;
    PC pcfieldsplit1, pcfieldsplit2;

    PCFieldSplitSetIS(pc, "0",is_U);//The order here matters a lot between this line and the next
    PCFieldSplitSetIS(pc, "1",is_P);//The order here matters a lot between this line and the previous
    PCFieldSplitSetSchurPre(pc,PC_FIELDSPLIT_SCHUR_PRE_SELFP,NULL);//or PC_FIELDSPLIT_SCHUR_PRE_USER if you provide Chat
    PCFieldSplitSetSchurFactType( pc, PC_FIELDSPLIT_SCHUR_FACT_FULL);
    PetscCall( PCSetUp( pc) );
    PCFieldSplitSchurGetSubKSP( pc, NULL, &kspArray);
    KSPSetType( kspArray[0], KSPPREONLY);
    KSPSetType( kspArray[1], KSPPREONLY);
    KSPGetPC(kspArray[0], &pcfieldsplit1);
    KSPGetPC(kspArray[1], &pcfieldsplit2);

    PCSetType( pcfieldsplit1, PCJACOBI);
    PCSetType( pcfieldsplit2, PCHYPRE);
    PetscCall( PCHYPRESetType( pcfieldsplit2, "boomeramg") );

    PetscCall(PetscOptionsSetValue(NULL, "-fieldsplit_1_pc_hypre_boomeramg_strong_threshold", "0.8") );
    PetscCall(PetscOptionsSetValue(NULL, "-fieldsplit_1_pc_hypre_boomeramg_agg_nl",  "4") );
    /*
    PetscCall(PetscOptionsSetValue(NULL, "-fieldsplit_1_pc_hypre_boomeramg_agg_num_paths", "5") );
    PetscCall(PetscOptionsSetValue(NULL, "-fieldsplit_1_pc_hypre_boomeramg_max_levels", "25") );
    PetscCall(PetscOptionsSetValue(NULL, "-fieldsplit_1_pc_hypre_boomeramg_coarsen_type", "PMIS") );
    PetscCall(PetscOptionsSetValue(NULL, "-fieldsplit_1_pc_hypre_boomeramg_interp_type", "ext+i") );
    PetscCall(PetscOptionsSetValue(NULL, "-fieldsplit_1_pc_hypre_boomeramg_P_max", "2") );
    PetscCall(PetscOptionsSetValue(NULL, "-fieldsplit_1_pc_hypre_boomeramg_truncfactor", "0.5") );
    */
    PetscCall( PCSetFromOptions(pcfieldsplit2) );//KSPSetFromOptions ne passe pas les options à pcfieldsplit2


    PetscCall( KSPSetFromOptions(ksp) );
    PetscCall( KSPSetUp(ksp) );
    PetscPrintf(PETSC_COMM_WORLD,"Solving the linear system A_input*X_output = b_input with a Schur preconditioner...\n");

    PetscCall( KSPSolve(ksp,b_input, *X_output) );

    //Extract and display informations about the resolution
    displayPCFieldSplitIterationNumbers(&ksp, residu);
    
    KSPDestroy(&ksp);
    PetscFree(kspArray);

    return PETSC_SUCCESS;
}

//##### Solve the left transformed system for Xoutput
int solveLeftTransformedSystemForXoutput( Mat Ahat, Mat Pmat, IS is_U, IS is_P, Vec b_hat, Vec * X_output, PetscReal rtol, PetscReal abstol, PetscReal dtol, PetscInt numberMaxOfIter, double * residu, PetscBool useLowerTriangularTransform)
{
    KSP ksp, *kspArray;
    PC pc, pc1, pc2;
    KSPType ksp_type = KSPFBCGS;//FBCGS seems much more efficient than FGMRES
    PCType pc_type=PCFIELDSPLIT;
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
    PetscCall( PCSetUp( pc) );
    PCFieldSplitGetSubKSP( pc, NULL, &kspArray);
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
    PetscPrintf(PETSC_COMM_WORLD,"Solving the linear system A_input*X_output = b_hat with a fieldsplit preconditioner...\n");

    PetscCall( KSPSolve(ksp,b_hat, *X_output) );

    //Extract and display informations about the convergence
    displayPCFieldSplitIterationNumbers( &ksp, residu);
    
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
int transformSystemLeftRight( Mat M, Mat G, Mat D, Mat C, Mat * A_hat, Mat * Pmat, Vec * v)
{
    PetscPrintf(PETSC_COMM_WORLD,"Transformation of the original system matrix A_input by multiplication to the left and the right by upper and lower triangular matrices U and L : Ahat = L*A_input*U ...\n");

    Vec v_redistributed;
    Mat D_M_inv_G, D_DM_inv, Mat_array[4];// D_M_inv = diag(M)^{-1}, D_DM_inv = D*diag(M)^{-1}
    Mat C_hat, G_hat, D_hat, C_hat2, diag_2M;
    VecScatter scat;//tool to redistribute a vector on the processors
    IS is_to, is_from;
    PetscInt col_min, col_max;

    //Extraction of the diagonal of M
    MatCreateVecs(M,NULL,v);//v has the size of M
    MatGetDiagonal(M,*v);

    //Creation of matrix 2*diag(M). Why not use MatCreateDiagonal ???
    PetscCall( MatDuplicate(M, MAT_DO_NOT_COPY_VALUES, &diag_2M) );
    MatEliminateZeros(diag_2M, PETSC_TRUE);
    MatDiagonalSet(diag_2M, *v,  INSERT_VALUES);
    MatScale(diag_2M,2);//store 2*diagonal part of M
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
    MatMatMult(D,D_M_inv_G,MAT_INITIAL_MATRIX,PETSC_DEFAULT,&C_hat);//C_hat contains D*D_M_inv*G
    MatAYPX(C_hat,-1.0,C,SUBSET_NONZERO_PATTERN);//C_hat contains C - D*D_M_inv*G

    // Creation of G_hat
    MatMatMult(M,D_M_inv_G,MAT_INITIAL_MATRIX,PETSC_DEFAULT,&G_hat);//G_hat contains M*D_M_inv*G
    MatAYPX(G_hat,-1.0,G,UNKNOWN_NONZERO_PATTERN);//G_hat contains G - M*D_M_inv*G

    // Creation of D_hat
    MatMatMult(D_DM_inv,M,MAT_INITIAL_MATRIX,PETSC_DEFAULT,&D_hat);//D_hat contains D*DM_inv*M
    MatAYPX(D_hat,-1.0,D,UNKNOWN_NONZERO_PATTERN);//D_hat contains D - D*DM_inv*M

    // Creation of C_hat2
    MatMatMult(D_DM_inv, G_hat,MAT_INITIAL_MATRIX,PETSC_DEFAULT,&C_hat2);//C_hat2 contains D*D_M_inv*Ghat
    MatAYPX(C_hat2,-1.0,C_hat,SUBSET_NONZERO_PATTERN);//C_hat2 contains Chat - D*D_M_inv*Ghat

    //Creation of global matrices using MatCreateNest
    Mat_array[3]=C_hat2;//Bottom right block of A_hat
    Mat_array[2]=D_hat;//Bottom left block of A_hat
    Mat_array[1]=G_hat;//Top right block of A_hat
    Mat_array[0]=M;//Top left block of A_hat

    // Creation of A_hat
    MatCreateNest(PETSC_COMM_WORLD,2,NULL,2,NULL,Mat_array,A_hat);

    // Creation of Pmat
    Mat_array[0]=diag_2M;//Replace M by its diagonal to ease inversion
    Mat_array[3]=C_hat;//Bottom right block of Pmat
    MatCreateNest(PETSC_COMM_WORLD,2,NULL,2,NULL,Mat_array,Pmat);

    PetscPrintf(PETSC_COMM_WORLD,"... matrix transformed \n");    

    MatDestroy(&diag_2M);
    MatDestroy(&C_hat);
    MatDestroy(&C_hat2);
    MatDestroy(&G_hat);
    MatDestroy(&D_hat);
    MatDestroy(&D_M_inv_G);
    VecScatterDestroy(&scat);
    VecDestroy(&v_redistributed);
}

//##### Solve the left-right transformed system for Xhat
int solveLeftRightTransformedSystemForXhat( Mat A_hat, Mat Pmat, IS is_U, IS is_P, Vec b_hat, Vec * X_hat, PetscReal rtol, PetscReal abstol, PetscReal dtol, PetscInt numberMaxOfIter, double * residu)
{
    KSP ksp, *kspArray;
    PC pc, pc1, pc2;
    KSPType ksp_type = KSPFBCGS;//BCGS seems very efficient
    PCType pc_type=PCFIELDSPLIT;
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
    PetscCall( PCSetUp( pc) );
    PCFieldSplitGetSubKSP( pc, NULL, &kspArray);
    KSPSetType( kspArray[0], KSPPREONLY);
    KSPSetType( kspArray[1], KSPPREONLY);
    KSPGetPC(kspArray[0], &pc1);
    KSPGetPC(kspArray[1], &pc2);

    PCSetType( pc1, PCJACOBI);
    PCSetType( pc2, PCGAMG);

    PetscCall( KSPSetFromOptions(ksp) );
    PetscCall( KSPSetUp(ksp) );
    PetscPrintf(PETSC_COMM_WORLD,"Solving the linear system A_hat*X_hat = b_hat with a fieldsplit preconditioner ...\n");

    PetscCall( KSPSolve(ksp,b_hat, *X_hat) );

    displayPCFieldSplitIterationNumbers( &ksp, residu);

    KSPDestroy(&ksp);
    PetscFree(kspArray);

    return PETSC_SUCCESS;
}

int displayPCFieldSplitIterationNumbers(KSP *ksp, double *residu)
{
    KSP *kspArray;
    PC pc, pc1, pc2;
    KSPType ksp_type0, ksp_type1,  ksp_type;
    PCType pc_type, pc_type0, pc_type1;
    int nblocks, iter, iter1, iter2;//iter = main iteration number, iter1 and iter2 are sub iteration numbers
    KSPConvergedReason reason;
    PetscReal rtol, abstol, dtol;
    PetscInt numberMaxOfIter;
    PetscBool isPCFIELDSPLIT;
    
    KSPGetConvergedReason(*ksp,&reason);
    KSPGetResidualNorm( *ksp, residu);
    KSPGetTolerances( *ksp, &rtol, &abstol, &dtol, &numberMaxOfIter);
    KSPGetPC(*ksp,&pc);
    KSPGetType( *ksp, &ksp_type);
    PCGetType( pc, &pc_type);
    PetscCall(PetscStrcmp(pc_type ,PCFIELDSPLIT, &isPCFIELDSPLIT));
    PetscCheck( isPCFIELDSPLIT, MPI_COMM_WORLD, PETSC_ERR_ARG_OUTOFRANGE, "Display function displayPCFieldSplitIterationNumbers works for PCFIELDSPLIT preconditioners, pc_type = %s", pc_type);
    PetscCall( PCFieldSplitGetSubKSP( pc, &nblocks, &kspArray) );
    PetscCheck( nblocks == 2, MPI_COMM_WORLD, PETSC_ERR_ARG_OUTOFRANGE, "Display function displayPCFieldSplitIterationNumbers works for 2 blocks in PCFIELDSPLIT preconditioners, nblocks = %d", nblocks);
    KSPGetType( kspArray[0], &ksp_type0);
    KSPGetType( kspArray[1], &ksp_type1);
    KSPGetPC(kspArray[0],&pc1);
    KSPGetPC(kspArray[1],&pc2);
    PCGetType( pc1, &pc_type0);
    PCGetType( pc2, &pc_type1);
    KSPGetIterationNumber(*ksp,&iter);
    KSPGetIterationNumber(kspArray[0],&iter1);
    KSPGetIterationNumber(kspArray[1],&iter2);

    if (reason>0)
    {
        PetscPrintf(PETSC_COMM_WORLD, "\n############ : monitoring the convergence of the linear solver\n");
        PetscPrintf(PETSC_COMM_WORLD, "Linear solver name: %s, preconditioner %s, %d iterations \n", ksp_type, pc_type, iter);
        PetscPrintf(PETSC_COMM_WORLD, "    sub solver 1 name : %s, preconditioner %s, %d iterations \n", ksp_type0, pc_type0, iter1);
        PetscPrintf(PETSC_COMM_WORLD, "    sub solver 2 name : %s, preconditioner %s, %d iterations \n", ksp_type1, pc_type1, iter2);
    }
    else
        PetscPrintf(PETSC_COMM_WORLD, "!!!!!!!!!!!!!!!!!! Linear system diverged  after %d iterations !!!!!!!!!!!!!!\n", iter);
        
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
}

int displayPCCompositeIterationNumbers(KSP *ksp, double *residu)
{
    KSP *kspArray;
    PC pc, pc1, pc2;
    KSPType ksp_type0, ksp_type1,  ksp_type;
    PCType pc_type, pc_type0="none", pc_type1="none";
    int nprecs, iter, iter1, iter2;//iter = main iteration number, iter1 and iter2 are sub iteration numbers
    KSPConvergedReason reason;
    PetscReal rtol, abstol, dtol;
    PetscInt numberMaxOfIter;
    PetscBool isPCFIELDSPLIT;
    
    KSPGetConvergedReason(*ksp,&reason);
    KSPGetResidualNorm( *ksp, residu);
    KSPGetTolerances( *ksp, &rtol, &abstol, &dtol, &numberMaxOfIter);
    KSPGetPC(*ksp,&pc);
    PCGetType( pc, &pc_type);
    PetscBool isPCComposite;
    PetscCall(PetscStrcmp(pc_type ,PCCOMPOSITE, &isPCComposite));
    PetscCheck( isPCComposite, MPI_COMM_WORLD, PETSC_ERR_ARG_OUTOFRANGE, "Display function  displayPCCompositeIterationNumbers works for PCCOMPOSITE preconditioners, pc_type = %s, PCCOMPOSITE = %s", pc_type, PCCOMPOSITE);
    PCCompositeGetNumberPC( pc, &nprecs);
    PetscCheck( nprecs >= 1, MPI_COMM_WORLD, PETSC_ERR_ARG_OUTOFRANGE, "Display function displayPCCompositeIterationNumbers works for at least 1 sub preconditioners, nprecs = %d", nprecs);
    PCCompositeGetPC( pc, 0, &pc1);
    KSPGetType( *ksp, &ksp_type);
    PCGetType( pc1, &pc_type0);
    KSPGetIterationNumber(*ksp,&iter);
    if(nprecs >= 2)
    {
        PCCompositeGetPC( pc, 1, &pc2);
        PCGetType( pc2, &pc_type1);        
    }
    if (reason>0)
    {
        PetscPrintf(PETSC_COMM_WORLD, "\n############ : monitoring the convergence of the linear solver with %d (composed) preconditioner(s)\n", nprecs);
        PetscPrintf(PETSC_COMM_WORLD, "Linear solver name: %s, preconditioner %s, %d iterations \n", ksp_type, pc_type, iter);
        PetscPrintf(PETSC_COMM_WORLD, "    composite preconditioner 0 name : %s \n", pc_type0);
        PetscCall(PetscStrcmp(pc_type0 ,PCFIELDSPLIT, &isPCFIELDSPLIT));
        if( isPCFIELDSPLIT )
          displayPCFieldSplitSubTypes(pc1);
        PetscPrintf(PETSC_COMM_WORLD, "    composite preconditioner 1 name : %s \n", pc_type1);
        PetscCall(PetscStrcmp(pc_type1 ,PCFIELDSPLIT, &isPCFIELDSPLIT));
        if( isPCFIELDSPLIT )
          displayPCFieldSplitSubTypes( pc2);
    }
    else
        PetscPrintf(PETSC_COMM_WORLD, "!!!!!!!!!!!!!!!!!! Linear system diverged  after %d iterations !!!!!!!!!!!!!!\n", iter);
        
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
        case -9:
            PetscPrintf(PETSC_COMM_WORLD, "!!!!!!! Nan or Inf in the linear solver!!!!!! \n");
            break;
        default:
            if (reason>0)
                PetscPrintf(PETSC_COMM_WORLD, "PETSc convergence reason %d \n", reason);
            else
                PetscPrintf(PETSC_COMM_WORLD, "PETSc divergence reason %d \n" , reason);
        }
}

int displayPCFieldSplitSubTypes(PC pc)
{
    KSP *kspArray;
    PC pc1, pc2;
    KSPType ksp_type0,  ksp_type1;
    PCType pc_type, pc_type0, pc_type1;
    int nblocks, iter, iter1, iter2;//iter = main iteration number, iter1 and iter2 are sub iteration numbers
    PetscBool isPCFIELDSPLIT;

    PCGetType( pc, &pc_type);
    PetscCall(PetscStrcmp(pc_type ,PCFIELDSPLIT, &isPCFIELDSPLIT));
    PetscCheck( isPCFIELDSPLIT, MPI_COMM_WORLD, PETSC_ERR_ARG_OUTOFRANGE, "Display function displayPCFieldSplitSubTypes works for PCFIELDSPLIT preconditioners, pc_type = %s", pc_type);
    PetscCall( PCFieldSplitGetSubKSP( pc, &nblocks, &kspArray) );
    PetscCheck( nblocks == 2, MPI_COMM_WORLD, PETSC_ERR_ARG_OUTOFRANGE, "Display function displayPCFieldSplitSubTypes works for 2 blocks in PCFIELDSPLIT preconditioners, nblocks = %d", nblocks);
    KSPGetType( kspArray[0], &ksp_type0);
    KSPGetType( kspArray[1], &ksp_type1);
    KSPGetPC(kspArray[0],&pc1);
    KSPGetPC(kspArray[1],&pc2);
    PCGetType( pc1, &pc_type0);
    PCGetType( pc2, &pc_type1);
    KSPGetIterationNumber(kspArray[0],&iter1);
    KSPGetIterationNumber(kspArray[1],&iter2);

    PetscPrintf(PETSC_COMM_WORLD, "       fieldsplit sub solver 1 name : %s, preconditioner %s, %d iterations \n", ksp_type0, pc_type0, iter1);
    PetscPrintf(PETSC_COMM_WORLD, "       fieldsplit sub solver 2 name : %s, preconditioner %s, %d iterations \n", ksp_type1, pc_type1, iter2);
}
