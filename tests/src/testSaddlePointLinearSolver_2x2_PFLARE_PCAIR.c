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

//##### Symmetric diagonal (Jacobi) scaling of the whole outer matrix, before any block extraction.
//      Replace A by S*A*S with S = diag(1/sqrt(|A_ii|)); rows with a zero diagonal are left unscaled.
//      Done in place, so the extracted blocks, the RHS (built from the chosen exact solution)
//      and the reported error all remain consistent with the scaled system (no unscaling needed).
	Vec diagScale;
	MatCreateVecs(A_input, &diagScale, NULL);
	MatGetDiagonal(A_input, diagScale);
	{
		PetscInt     nScaleLoc;
		PetscScalar *scaleArray;
		VecGetLocalSize(diagScale, &nScaleLoc);
		VecGetArray(diagScale, &scaleArray);
		for (PetscInt i = 0; i < nScaleLoc; i++) {
			PetscReal d = PetscAbsScalar(scaleArray[i]);
			scaleArray[i] = d > 0.0 ? 1.0/PetscSqrtReal(d) : 1.0;
		}
		VecRestoreArray(diagScale, &scaleArray);
	}
	MatDiagonalScale(A_input, diagScale, diagScale);
	VecDestroy(&diagScale);
	PetscPrintf(PETSC_COMM_WORLD,"Applied symmetric diagonal scaling S*A*S to the outer matrix\n");

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
   KSPSetNormType(ksp,KSP_NORM_UNPRECONDITIONED);
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

   /*
   RUN WITH:
./compile_pflare_test.sh && ./tests/src/testSaddlePointLinearSolver_2x2_Schur_PFLARE     -f0 ./Matrix_CEA_2x2.petsc -nU 20344 -nP 9552 -on_error_abort -ksp_monitor -ksp_view -use_airg -fieldsplit_0_ksp_monitor -fieldsplit_0_pc_air_diag_scale_polys -fieldsplit_0_pc_air_coarsest_diag_scale_polys -fieldsplit_0_pc_air_cf_splitting_type diag_dom -fieldsplit_0_pc_air_a_lump -fieldsplit_0_pc_air_a_drop 1e-6 -fieldsplit_0_pc_air_r_drop 1e-4 -fieldsplit_0_pc_air_strong_threshold 0.2 -fieldsplit_0_pc_air_print_stats_timings -fieldsplit_1_ksp_monitor -fieldsplit_1_ksp_max_it 5 -fieldsplit_0_ksp_max_it 2   
   
   */

	PetscBool useJacobiInv = PETSC_TRUE;
	PetscBool useAirgInexact = PETSC_FALSE;//-use_airg : solve the velocity block M inexactly with AIRG instead of an approximate-inverse apply
	PetscReal airgInnerRtol   = 1.0e-2;    //-airg_inner_rtol   : relative tolerance (only binds if more than one V-cycle is allowed)
	PetscInt  airgInnerMaxits = 1;         //-airg_inner_maxits : number of AIRG V-cycles (undamped Richardson iterations)
	PetscOptionsGetBool(NULL, NULL, "-use_airg",         &useAirgInexact,  NULL);
	PetscOptionsGetReal(NULL, NULL, "-airg_inner_rtol",  &airgInnerRtol,   NULL);
	PetscOptionsGetInt (NULL, NULL, "-airg_inner_maxits",&airgInnerMaxits, NULL);

    if (useAirgInexact)//Solve M inexactly with AIRG, and solve the FULL Schur complement matrix-free (see Schur setup below)
    {
        //No assembled M^-1 is needed here for the Schur OPERATOR : it is solved matrix-free further down,
        //reusing this AIRG velocity solve for the M^-1 action (a cheap explicit Sp is built there, but only as a preconditioner).

        //Velocity sub-solve : a single AIRG V-cycle applied as one undamped Richardson iteration
        //(KSPRICHARDSON, scale 1.0, max its = airgInnerMaxits, default 1), rather than a GMRES solve to a tolerance.
        //The outer solver is flexible (KSPFBCGS), so it tolerates this fixed inner preconditioner.
        //These are only DEFAULTS : the sub-KSP carries the "fieldsplit_0_" prefix, so the KSPSetFromOptions
        //call below lets any -fieldsplit_0_* command-line options (incl. PCAIR -fieldsplit_0_pc_air_*) override them.
        KSPSetType( kspArray[0], KSPRICHARDSON);
        KSPRichardsonSetScale( kspArray[0], 1.0);//undamped Richardson
        KSPSetTolerances( kspArray[0], airgInnerRtol, PETSC_DEFAULT, PETSC_DEFAULT, airgInnerMaxits);
        PCSetType( pc1, PCAIR);
        KSPSetFromOptions( kspArray[0]);//apply -fieldsplit_0_* options (KSP + PCAIR) on top of the defaults above
        PetscCall( PCSetUp( pc1) );
        PetscPrintf(PETSC_COMM_WORLD,"Velocity block M solved by AIRG (PCAIR) via undamped Richardson; tune with -fieldsplit_0_* options.\n");
    }
    else if (useJacobiInv)//Extract the diagonal of M to build a diagonal approximate inverse used in the approximate Schur complement
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

        //Extract approximate inverse from Pflare
        PCPFLAREINVGetInverseMat( pc1, &Ap);
    }

    //Get the implicit Schur complement operator S (the Amat of the Schur sub-KSP).
    //Its M^-1 action is whatever the velocity sub-solve above provides (AIRG under -use_airg).
    KSPGetOperators( kspArray[1], &S  , &Sp);

    if (useAirgInexact)
    {
        //Matrix-free solve of the FULL Schur complement S = C - D M^-1 G, with M^-1 applied by the AIRG
        //velocity sub-solve. Because that inner solve is a fixed (undamped Richardson) linear operator,
        //S is a genuine linear operator, so a plain GMRES on the outside is well posed.
        //Preconditioned by the cheap explicit approximate Schur Sp = C - D diag(M)^-1 G (sparse, LU-factored).
        Vec v;
        MatCreateVecs(M,NULL,&v);
        MatGetDiagonal(M,v);
        VecReciprocal(v);
        MatCreateDiagonal( v, &Ap);
        VecDestroy(&v);
        getSchurComplement( Ap, G, D, C, &Sp );//explicit approx, used ONLY as the preconditioning matrix (Pmat)

        KSPSetType( kspArray[1], KSPGMRES);
        KSPSetOperators( kspArray[1], S, Sp);//operator = matrix-free full S ; Pmat = sparse approx Sp
        PCSetType( pc2, PCLU);//factor the sparse approximate Schur complement as the preconditioner
        KSPSetFromOptions( kspArray[1]);//allow -fieldsplit_1_* overrides (ksp type/tol, pc, monitors, ...)
        PetscPrintf(PETSC_COMM_WORLD,"Schur complement solved MATRIX-FREE on full S = C - D M^-1 G (AIRG M^-1), GMRES + LU(C - D diag(M)^-1 G) preconditioner; tune with -fieldsplit_1_* options\n");
    }
    else
    {
        //Build the explicit approximate Schur complement Sp = C - D Ap G and solve it directly (LU).
        getSchurComplement( Ap, G, D, C, &Sp );//Sparse Schur complement is built and stored in Sp
        KSPSetOperators( kspArray[1],   S,  Sp);//operator S (implicit), preconditioning matrix Sp (explicit approx)
        PCSetType( pc2, PCLU);//try PCGAMG, PCHYPRE and PCAIR
    }

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
