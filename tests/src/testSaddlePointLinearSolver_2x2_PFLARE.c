static char help[] = "Read a PETSc matrix from a file -f0 <input file>\n Parameters : \n -f0 : matrix fileName \n -nU :number of velocity lines \n -nP : number of pressure lines \n -mat_type : PETSc matrix type \n ";

/*****************************************************************************************************************/
/* Parallel implementation of a Schur type preconditioner for the linear system A_{input} X_{output} = b_{input} */
/*                                                                                                               */
/* Description : Parallel file with PC_FIELDSPLIT_SCHUR_PRE_USER type, restricted to 2x2 blocs.                  */
/*               Use of explicitly built approximate Schur complement based on PFLARE approximate inverse        */ 
/*               Requires PFLARE library in order to use PCPFLAREINV                                             */ 
/*                                                                                                               */
/* Input  : - Matrix A_{input}    (system matrix, loaded from a file)                                            */
/*          - Vector b_{input}    (right hand side, made up for testing : b = A_{input} X_{exact} )              */
/*                                                                                                               */
/* Output : - Vector X_{output}   (unknown vector, to be determined)                                             */
/*          - double error        (|| X_{exact} - X_{output} ||)                                                 */
/*                                                                                                               */
/* Auxilliary variables : - M top    left  submatrix of A_{input}                                */
/*                        - G top    right submatrix of A_{input}                                */
/*                        - D bottom left  submatrix of A_{input}                                */
/*                        - C bottom right submatrix of A_{input}                                */
/*                                                                                               */
/*                                 *M   G*                                                       */
/*                        A     = *       *                                                      */
/*                                 *D   C*                                                       */
/*                                                                                               */
/*****************************************************************************************************************/

#include <petscis.h>
#include <petscksp.h>
#include <math.h>
#include "pflare.h"

//PFLARE can return a MATDIAGONAL matrix (sparsity order 0). In this case we must convert MATDIAGONAL to MATAIJ.
//Schur matrix Sp is created, user should delete after use
int getSchurComplement( Mat Minv, Mat G, Mat D, Mat C, Mat * Sp )
{
    MatType mat_type;
    MatGetType( Minv, &mat_type);

    if( strcmp(mat_type , MATDIAGONAL) == 0 )//Careful since products are not allowed between MATDIAGONAL and MATAIJ
    {
        Mat Minv2;
        MatConvert(Minv, MATAIJ, MAT_INITIAL_MATRIX, &Minv2);
        PetscCall( MatMatMatMult( D, Minv2, G,  MAT_INITIAL_MATRIX, PETSC_DEFAULT, Sp) );//Creates matrix S
        MatDestroy(&Minv2);
    }
    else
        PetscCall( MatMatMatMult( D, Minv, G,  MAT_INITIAL_MATRIX, PETSC_DEFAULT, Sp) );//Creates matrix S

    PetscCall( MatAYPX(*Sp,-1.0,C,SUBSET_NONZERO_PATTERN) );//S contains C - D*M_inv*G
}

int main( int argc, char **args ){
	PetscInitialize(&argc,&args, (char*)0,help);
	PetscMPIInt    size;        /* size of communicator */
	PetscMPIInt    rank;        /* processor rank */
	MPI_Comm_rank(PETSC_COMM_WORLD,&rank);
	MPI_Comm_size(PETSC_COMM_WORLD,&size);

//##### Load the matrix A_{input} contained in the file given in the command line
	char file[1][PETSC_MAX_PATH_LEN], mat_type[256]; // File to load, matrix type
	PetscViewer viewer;
	Mat A_input;
	PetscBool flg;
	PetscInt n_u, n_p, n;//Total number of velocity and pressure lines. n = n_u+ n_p

	PetscOptionsGetString(NULL,NULL,"-f0",file[0],PETSC_MAX_PATH_LEN,&flg);
	PetscStrcpy(mat_type,MATAIJ);// Default value for PETSc Matrix type
	PetscOptionsGetString(NULL,NULL,"-mat_type",mat_type,sizeof(mat_type),NULL);

	PetscPrintf(PETSC_COMM_WORLD,"Loading Matrix type %s from file %s on %d processor(s)...\n", mat_type, file[0], size);	
	PetscViewerCreate(PETSC_COMM_WORLD, &viewer);	
	PetscViewerSetType(viewer,PETSCVIEWERBINARY);//Use PETSCVIEWERHDF5 for better parallel performance
	PetscViewerFileSetMode(viewer,FILE_MODE_READ);
	PetscViewerFileSetName(viewer,file[0]);
	
	PetscOptionsGetInt(NULL,NULL,"-nU",&n_u,NULL);
	PetscOptionsGetInt(NULL,NULL,"-nP",&n_p,NULL);
	n=n_u+n_p;

	MatCreate(PETSC_COMM_WORLD, &A_input);
	MatSetType(A_input,mat_type);

	if( size>1)
	    if( rank == 0)
	        MatSetSizes( A_input, n-(size-1)*((n-n_u)/(size-1)), n-(size-1)*((n-n_u)/(size-1)), n, n);
	    else
	        MatSetSizes( A_input, (n-n_u)/(size-1), (n-n_u)/(size-1), n, n);

	MatLoad(A_input,viewer);
	PetscViewerDestroy(&viewer);
	PetscPrintf(PETSC_COMM_WORLD,"... matrix Loaded \n");	
	PetscBarrier(NULL);

//####	Decompose the matrix A_input into 4 blocks M, G, D, C
	Mat M, G, D, C;
	PetscInt nrows, ncolumns;//Total number of rows and columns of A_input
	PetscInt irow_min, irow_max;//min and max indices of rows stored locally on this process
	IS is_U,is_P;

	MatGetOwnershipRange( A_input, &irow_min, &irow_max);
	MatGetSize( A_input, &nrows, &ncolumns);
	PetscInt min_pressure_lines = irow_min <= n_u ? n_u : irow_min;//max(irow_min, n_u)
	PetscInt max_velocity_lines = irow_max >= n_u ? n_u : irow_max;//min(irow_max, n_u)
	PetscInt nb_pressure_lines = irow_max >= n_u ? irow_max - min_pressure_lines : 0;
	PetscInt nb_velocity_lines = irow_min <= n_u ? max_velocity_lines - irow_min : 0;
	PetscInt nb_local_lines = irow_max - irow_min; 

	PetscCheck( nrows == ncolumns, PETSC_COMM_WORLD, PETSC_ERR_ARG_SIZ, "Matrix is not square !!!\n");
	PetscCheck( n == ncolumns, PETSC_COMM_WORLD, PETSC_ERR_ARG_SIZ, "Inconsistent data : the matrix has %d lines but only %d velocity lines and %d pressure lines declared\n", ncolumns, n_u,n_p);
	PetscPrintf(PETSC_COMM_WORLD,"The matrix has %d lines : %d velocity lines and %d pressure lines\n", n, n_u,n_p);
	PetscPrintf(PETSC_COMM_SELF,"Process %d has %d local rows : irow_min = %d, irow_max = %d, min_pressure_lines = %d, max_velocity_lines = %d, nb_pressure_lines = %d, nb_velocity_lines = %d \n", rank, nb_local_lines, irow_min, irow_max, min_pressure_lines, max_velocity_lines, nb_pressure_lines, nb_velocity_lines);
	
	PetscPrintf(PETSC_COMM_WORLD,"Extraction of the 4 blocks \n M G\n D C\n");
	ISCreateStride(PETSC_COMM_WORLD, nb_velocity_lines, max_velocity_lines - nb_velocity_lines, 1, &is_U);
	ISCreateStride(PETSC_COMM_WORLD, nb_pressure_lines, min_pressure_lines                    , 1, &is_P);
	
	MatCreateSubMatrix(A_input,is_U, is_U,MAT_INITIAL_MATRIX,&M);
	MatCreateSubMatrix(A_input,is_U, is_P,MAT_INITIAL_MATRIX,&G);
	MatCreateSubMatrix(A_input,is_P, is_U,MAT_INITIAL_MATRIX,&D);
	MatCreateSubMatrix(A_input,is_P, is_P,MAT_INITIAL_MATRIX,&C);
	PetscPrintf(PETSC_COMM_WORLD,"... end of extraction\n");

	MatGetOwnershipRange( M, &irow_min, &irow_max);
	PetscPrintf(PETSC_COMM_SELF,"Matrix M, Process %d local rows : irow_min = %d, irow_max = %d \n", rank, irow_min, irow_max);
	MatGetOwnershipRange( G, &irow_min, &irow_max);
	PetscPrintf(PETSC_COMM_SELF,"Matrix G, Process %d local rows : irow_min = %d, irow_max = %d \n", rank, irow_min, irow_max);
	MatGetOwnershipRange( D, &irow_min, &irow_max);
	PetscPrintf(PETSC_COMM_SELF,"Matrix D, Process %d local rows : irow_min = %d, irow_max = %d \n", rank, irow_min, irow_max);
	MatGetOwnershipRange( C, &irow_min, &irow_max);
	PetscPrintf(PETSC_COMM_SELF,"Matrix C, Process %d local rows : irow_min = %d, irow_max = %d \n", rank, irow_min, irow_max);

	//#Display some informations about the four blocs
	int size1, size2;
	MatGetSize(M, &size1,&size2);
	PetscPrintf(PETSC_COMM_WORLD,"Size of M : %d,%d\n", size1,size2);
	MatGetSize(G, &size1,&size2);
	PetscPrintf(PETSC_COMM_WORLD,"Size of G : %d,%d\n", size1,size2);
	MatGetSize(D, &size1,&size2);
	PetscPrintf(PETSC_COMM_WORLD,"Size of D : %d,%d\n", size1,size2);
	MatGetSize(C, &size1,&size2);
	PetscPrintf(PETSC_COMM_WORLD,"Size of C : %d,%d\n", size1,size2);
	
//##### Definition of the right hand side to test the preconditioner
	Vec b_input, X_exact;
	PetscScalar values[nb_local_lines];//To store the values
	PetscInt    indices[nb_local_lines];//To store the indices

	PetscPrintf(PETSC_COMM_WORLD,"Creation of the RHS, exact and numerical solution vectors...\n");
	MatCreateVecs( A_input,&b_input,&X_exact );// parallel distribution of vectors should optimise the computation A_input*X_exact=b_input

	for (int i = 0; i<nb_local_lines; i++){
		values[i] = 1.0/(i+irow_min+1);//valeur second membre à imposer ici
		indices[i]=i+irow_min;
	}
	
	VecSetValues(X_exact,nb_local_lines,indices,values,INSERT_VALUES);
	VecAssemblyBegin(X_exact );
	VecAssemblyEnd(  X_exact );
	VecNormalize( X_exact, NULL);

	MatMult( A_input, X_exact, b_input);
	PetscPrintf(PETSC_COMM_WORLD,"... vectors created \n");	


//##### Set preconditioner and KSP solver
	double residu, abstol, rtol=1e-7, dtol;
	int iter, iter1, iter2, numberMaxOfIter;
	int nblocks=2;
	KSP ksp;
	KSP * kspArray;
	KSPType type, type1, type2;
	PC pc, pc1, pc2;
	PCType pctype, pctype1, pctype2;
    Mat  Ap, S, Sp;
    PetscInt pflarePolyOrder=6, pflareSparsityOrder=1;
    PCPFLAREINVType invType = PFLAREINV_NEUMANN;//Try PFLAREINV_NEUMANN or PFLAREINV_ARNOLDI
    PCRegister_PFLARE();

	PetscPrintf(PETSC_COMM_WORLD,"Definition of the KSP solver to test the preconditioner...\n");
	KSPCreate(PETSC_COMM_WORLD,&ksp);
	KSPSetType(ksp,KSPFBCGS);//Or KSPFGMRES
	PetscCall( KSPSetOperators(ksp,A_input,A_input) );
	KSPSetTolerances(ksp,rtol,PETSC_DEFAULT,PETSC_DEFAULT, PETSC_DEFAULT);
	KSPGetPC(ksp,&pc);

	PetscPrintf(PETSC_COMM_WORLD,"Setting the preconditioner...\n");
	PCSetType(pc,PCFIELDSPLIT);
	PCFieldSplitSetType(pc,PC_COMPOSITE_SCHUR);
	PCFieldSplitSetIS(pc, "0",is_U);//The order here matters a lot between this line and the next
	PCFieldSplitSetIS(pc, "1",is_P);//The order here matters a lot between this line and the previous
    PCFieldSplitSetSchurFactType( pc, PC_FIELDSPLIT_SCHUR_FACT_FULL);
    PetscCall( PCSetUp( pc) );
	PCFieldSplitGetSubKSP( pc, &nblocks, &kspArray);
	KSPSetType( kspArray[0], KSPPREONLY);
	KSPSetType( kspArray[1], KSPPREONLY);
	KSPGetPC(kspArray[0], &pc1);
	KSPGetPC(kspArray[1], &pc2);

	PetscBool useJacobiInv = PETSC_TRUE;
    
    if (useJacobiInv)//Extract the diagonal of M to build a diagonal approximate inverse used in the approximate Schur complement
    {
        PCSetType( pc1, PCJACOBI);
        Vec v;
	    MatCreateVecs(M,NULL,&v);
	    MatGetDiagonal(M,v);
   	    VecReciprocal(v);
        MatCreateDiagonal( v, &Ap);
        VecDestroy(&v);
    }
    else//Use PFLARE to build the approximate inverse of M
    {
        PCSetType( pc1, PCPFLAREINV);// This will allow the build of Sp from PCPFLAREINV
        //Set the pflare parameters
        PCPFLAREINVSetPolyOrder(    pc1, pflarePolyOrder);
        PCPFLAREINVSetSparsityOrder(pc1, pflareSparsityOrder);
        PCPFLAREINVSetType( pc1, invType);
        PetscCall( PCSetUp( pc1) );
        PCPFLAREINVGetPolyOrder(    pc1, &pflarePolyOrder);
        PCPFLAREINVGetSparsityOrder(pc1, &pflareSparsityOrder);
        PCPFLAREINVGetType(pc1,  &invType);
        PetscPrintf(PETSC_COMM_WORLD,"Built an approximate inverse using PCPFLAREINV type = %d, polynomial order = %d, sparsity order = %d\n", invType, pflarePolyOrder, pflareSparsityOrder);
        PetscPrintf(PETSC_COMM_WORLD,"  (for the record PFLAREINV_ARNOLDI = %d, PFLAREINV_NEUMANN = %d, PFLAREINV_JACOBI = %d, PFLAREINV_WJACOBI = %d)\n\n", PFLAREINV_ARNOLDI, PFLAREINV_NEUMANN, PFLAREINV_JACOBI, PFLAREINV_WJACOBI);
        
        KSPGetOperators( kspArray[1], &S  , &Sp);//Here we get the Schur preconditioning matrix Sp
    
        //Extract approximate inverse from Pflare
        PCPFLAREINVGetInverseMat( pc1, &Ap);
    }
    
    //Build the approximate schur complement
    getSchurComplement( Ap, G, D, C, &Sp );//Sparse Schur complement is built and stord in Sp
    //Give approx schur complement to the schur system
    KSPSetOperators( kspArray[1],   S,  Sp);  

    //Call KSPSolve
    PCSetType( pc2, PCLU);//try PCGAMG, PCHYPRE and PCAIR
    PetscCall( PCSetUp( pc) );
	KSPSetFromOptions(ksp);
	KSPSetUp(ksp);
	PetscPrintf(PETSC_COMM_WORLD,"Solving the linear system...\n");
	Vec X_output;//The unknown vector
	VecDuplicate(X_exact,&X_output);
	KSPSolve(ksp,b_input,X_output);

	//Extract informations about the convergence
	KSPConvergedReason reason;
	KSPGetConvergedReason(ksp,&reason);
	KSPGetIterationNumber(ksp,&iter);
	
	if (reason>0)
		PetscPrintf(PETSC_COMM_WORLD, "\nLinear system converged in %d iterations \n", iter);
	else
		PetscPrintf(PETSC_COMM_WORLD, "\n!!!!!!!!!!!!!!!!!! Linear system diverged  after %d iterations !!!!!!!!!!!!!!\n", iter);
		
	KSPGetResidualNorm( ksp, &residu);
	KSPGetTolerances( ksp, &rtol, &abstol, &dtol, &numberMaxOfIter);
	PCFieldSplitGetSubKSP( pc, &nblocks, &kspArray);
	KSPGetType( ksp, &type);
	KSPGetType( kspArray[0], &type1);
	KSPGetType( kspArray[1], &type2);
	KSPGetIterationNumber(kspArray[0],&iter1);
	KSPGetIterationNumber(kspArray[1],&iter2);
	KSPGetPC(kspArray[0],&pc1);
	KSPGetPC(kspArray[1],&pc2);
	PCGetType( pc, &pctype);
	PCGetType( pc1, &pctype1);
	PCGetType( pc2, &pctype2);
	PetscFree(kspArray);

	PetscPrintf(PETSC_COMM_WORLD, "\n############ : monitoring of the linear solver \n");
	PetscPrintf(PETSC_COMM_WORLD, "Linear solver name: %s, preconditioner %s, %d iterations \n", type, pctype, iter);
	PetscPrintf(PETSC_COMM_WORLD, "    sub solver 1 name : %s, preconditioner %s, %d iterations \n", type1, pctype1, iter1);
	PetscPrintf(PETSC_COMM_WORLD, "    sub solver 2 name : %s, preconditioner %s, %d iterations \n", type2, pctype2, iter2);

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

//##### Compute the error and check it is small
	Vec X_p;//Pressure components of the main unknown
	Vec X_u;//Velocity components of the main unknown
	Vec X_exact_p, X_exact_u;//Pressure and velocity components of the exact solution
	double error, error_p, error_u;
	
	VecGetSubVector( X_output, is_P, &X_p);
	VecGetSubVector( X_output, is_U, &X_u);
	VecGetSubVector( X_exact, is_P, &X_exact_p);
	VecGetSubVector( X_exact, is_U, &X_exact_u);

	VecAXPY(  X_p, -1, X_exact_p);
	VecNorm(  X_p, NORM_2, &error_p);
	PetscPrintf(PETSC_COMM_WORLD,"L2 Error on pressure p : ||X_exact_p - X_num_p|| = %e\n", error_p);
	VecAXPY(  X_u, -1, X_exact_u);
	VecNorm(  X_u, NORM_2, &error_u);
	PetscPrintf(PETSC_COMM_WORLD,"L2 Error on velocity u : ||X_exact_u - X_num_u|| = %e \n", error_u);

	error=sqrt(error_u*error_u+error_p*error_p);
	PetscPrintf(PETSC_COMM_WORLD,"L2 total Error : ||X_exact - X_num|| = %e, (remember ||X_exact||=1)\n\n", error);

	PetscCheck( error < 1e6*residu, PETSC_COMM_WORLD, PETSC_ERR_NOT_CONVERGED, "Linear system did not return accurate solution. Error is too high compared to residual (e>1e6*r) : e=%e, r=%e\n", error, residu);

//##### Save the results in a JSON file
	#include <unistd.h>
	#include <fcntl.h>
	#include <sys/stat.h>

    printf("Creating output directory tmp...\n");
    if (access("tmp", F_OK) == -1) {
        if (mkdir("tmp", 0777) == -1) {
            perror("Error creating directory tmp");
            return 1;
        }
    }
	FILE* outputFile = fopen("tmp/output.json", "w");
	fprintf(outputFile, "{\n");
	fprintf(outputFile, "  \"iter\": %d,\n", iter);
	fprintf(outputFile, "  \"iter1\": %d,\n", iter1);
	fprintf(outputFile, "  \"iter2\": %d,\n", iter2);
	fprintf(outputFile, "  \"residual\": %.6e,\n", residu);
	fprintf(outputFile, "  \"total-error\": %.6e,\n", error);
	fprintf(outputFile, "  \"pressure-error\": %.6e,\n", error_u);
	fprintf(outputFile, "  \"velocity-error\": %.6e,\n", error_p);
	fprintf(outputFile, "  \"computation-time\": %d,\n", -42);
	fprintf(outputFile, "  \"memory-consumption\": %d\n", -42);
	fprintf(outputFile, "}\n");
	fclose(outputFile);
	printf("testOutput saved in tmp/output.json\n");

//##### Cleaning of the code
	MatDestroy(&M);
	MatDestroy(&D);	
	MatDestroy(&G);
	MatDestroy(&C);
	MatDestroy(&A_input);
    
	VecDestroy(&b_input);
	VecDestroy(&X_exact);
	VecDestroy(&X_u);
	VecDestroy(&X_p);
	VecDestroy(&X_output);
	
	ISDestroy(&is_U);
	ISDestroy(&is_P);

	KSPDestroy(&ksp);
	PetscFree(kspArray);

	PetscFinalize();
	return 0;
}
