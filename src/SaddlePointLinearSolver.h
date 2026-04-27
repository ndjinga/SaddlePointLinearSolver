#include <petscis.h>
#include <petscksp.h>

/* Fonctions principales */
void loadPETScMat(char* file, char* mat_type, Mat * A, PetscInt n_u, PetscInt n_p);
int splitPETScMatrix2x2(Mat A_input, PetscInt n_u, PetscInt n_p, Mat * M, Mat * G, Mat *D, Mat * C, IS * is_U, IS * is_P);
void buildRHSVector( Mat A_input, PetscInt n_u, PetscInt n_p, Vec * X_anal, Vec * b_input);
int transformSystemRight( Mat M, Mat G, Mat D, Mat C, Mat * A_hat, Mat * Pmat, Vec * v);
int transformSystemLeft(  Mat M, Mat G, Mat D, Mat C, Mat * A_hat, Mat * Pmat, Vec * v, PetscBool useLowerTriangularTransform);
int transformSystemLeftRight( Mat M, Mat G, Mat D, Mat C, Mat * A_hat, Mat * Pmat, Vec * v);
int getAhatRight( Mat M, Mat G, Mat D, Mat C, Mat * A_hat );
int getChatRight( Mat M, Mat G, Mat D, Mat C, Mat * C_hat );
int solveRightTransformedSystemForXhat(   Mat Ahat, Mat Pmat, IS is_U, IS is_P, Vec b_input, Vec * X_hat,    PetscReal rtol, PetscReal abstol, PetscReal dtol, PetscInt numberMaxOfIter, double * residu);
int solveLeftTransformedSystemForXoutput( Mat Ahat, Mat Pmat, IS is_U, IS is_P, Vec b_hat,   Vec * X_output, PetscReal rtol, PetscReal abstol, PetscReal dtol, PetscInt numberMaxOfIter, double * residu, PetscBool useLowerTriangularTransform);
int solveLeftRightTransformedSystemForXhat(   Mat Ahat, Mat Pmat, IS is_U, IS is_P, Vec b_hat, Vec * X_hat,    PetscReal rtol, PetscReal abstol, PetscReal dtol, PetscInt numberMaxOfIter, double * residu);
int solveRightILUTransformedSystemForXoutput( Mat A_input, Mat A_hat, Mat M, Mat G, IS is_U, IS is_P, Vec b_input, Vec * X_output, PetscReal rtol, PetscReal abstol, PetscReal dtol, PetscInt numberMaxOfIter, double *residu);
int solveSchurSystemForXoutput( Mat A_input, IS is_U, IS is_P, Vec b_input, Vec * X_output, PetscReal rtol, PetscReal abstol, PetscReal dtol, PetscInt numberMaxOfIter, double *residu);
int solveSchurHypreSystemForXoutput( Mat A_input, IS is_U, IS is_P, Vec b_input, Vec * X_output, PetscReal rtol, PetscReal abstol, PetscReal dtol, PetscInt numberMaxOfIter, double *residu);
void getSolutionFromXhat(Mat G, Vec v, Vec X_hat, Vec * X_output, Vec * X_u, Vec * X_p, IS is_U, IS is_P);
int getbhatFrombinput(Mat D, Vec v, Vec b_input, Vec * b_hat, IS is_U, IS is_P, PetscBool useLowerTriangularTransform);
double computeErrorAndCheck( Vec X_anal, Vec X_output, IS is_U, IS is_P, Vec X_u, Vec X_p);


/* Fonctions utilitaires*/
int displayPCFieldSplitIterationNumbers(KSP *ksp, double *residu);
int displayPCCompositeIterationNumbers(KSP *ksp, double *residu);
int displayPCFieldSplitSubTypes(PC pc1);
PetscErrorCode setupRight(PC pcshell);
PetscErrorCode destroyRight(PC pcshell);
PetscErrorCode applyRight(PC pcshell, Vec x, Vec y);
