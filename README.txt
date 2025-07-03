 - pour localiser petsc
export PETSC_DIR=/volatile/catA/ndjinga/Logiciels/Petsc/petsc-3.21.0
export PETSC_ARCH=arch-linux-c-debug

- pour compiler 
make SaddlePointLinearSolver
make SaddlePointLinearSolver_Shat
make SaddlePointLinearSolver_par

- pour exécuter façon 1

./SaddlePointLinearSolver -f0 Matrices/Assembly_PolyMAC_1_vp.petsc -nU 20344 -nP 9552 -pc_fieldsplit_type schur -pc_fieldsplit_schur_fact_type lower -fieldsplit_0_ksp_type gmres -fieldsplit_0_pc_type gamg -fieldsplit_0_pc_gamg_type agg -fieldsplit_1_ksp_type preonly -fieldsplit_1_pc_type lu -on_error_attach_debugger

- exécuter façon 2
./SaddlePointLinearSolver_Shat -f0 Matrices/Assembly_PolyMAC_1_vp.petsc -nU 20344 -nP 9552 -pc_fieldsplit_type schur -pc_fieldsplit_schur_fact_type lower -fieldsplit_0_ksp_type gmres -fieldsplit_0_pc_type gamg -fieldsplit_0_pc_gamg_type agg -fieldsplit_1_ksp_type preonly -fieldsplit_1_pc_type lu

OK, quand on appelle Schur avec une matrice de préconditionement triangulaire le complément de Schur est égal au bloc correspondant (noter que full=upper dans ce cas). l'option schur correspond donc bien à l'algorithme proposé par Pierre-Loïc. Cependant cette implémentation n'est pas du tout lisible pour le développeur et de plus l'option Schur parait est limitée au bloc 2x2. block Gauss-Seidel parait plus simple et naturel mais quels sont les mots clés ?
On peut utiliser -pc_fieldsplit_type multiplicative si la matrice est triangulaire inférieure. Il n'y a pas l'équivalent pour une matrice triangulaire supérieure. En cas de matrice triangulaire supérieure il faut soit faire symmetric_multiplicative soit mieux transposer

- exécuter façon 4
./SaddlePointLinearSolver_par -f0 Matrices/Assembly_PolyMAC_1_vp.petsc -nU 20344 -nP 9552 -pc_fieldsplit_type symmetric_multiplicative -fieldsplit_0_ksp_type gmres -fieldsplit_0_pc_type gamg -fieldsplit_0_pc_gamg_type agg -fieldsplit_1_ksp_type preonly -fieldsplit_1_pc_type lu

- exécuter façon 5
./SaddlePointLinearSolver -f0 Matrices/Assembly_PolyMAC_1_vp.petsc -nU 20344 -nP 9552 -pc_fieldsplit_type multiplicative -fieldsplit_0_ksp_type gmres -fieldsplit_0_pc_type gamg -fieldsplit_0_pc_gamg_type agg -fieldsplit_1_ksp_type preonly -fieldsplit_1_pc_type lu -on_error_attach_debugger

