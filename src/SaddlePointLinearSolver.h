#include <petscis.h>
#include <petscksp.h>

void loadPETScMat(char* file, char* mat_type, Mat * A, PetscInt n_u, PetscInt n_p);
int splitPETScMatrix2x2(Mat A_input, PetscInt n_u, PetscInt n_p, Mat * M, Mat * G, Mat *D, Mat * C, IS * is_U, IS * is_P);
void buildRHSVector( Mat A_input, PetscInt n_u, PetscInt n_p, Vec * X_anal, Vec * b_input);
void transformSaddlePointMatrix( Mat M, Mat G, Mat D, Mat C, Mat * A_hat, Mat * Pmat, Mat * C_hat, Mat * G_hat, Mat * diag_2M, Vec * v);
int solveTransformedSystemForXhat( Mat Amat, Mat Pmat, IS is_U, IS is_P, Vec b_hat, Vec * X_hat, PetscReal rtol, PetscReal abstol, PetscReal dtol, PetscInt numberMaxOfIter);
void getSolutionFromXhat(Mat G, Vec v, Vec X_hat, Vec * X_output, Vec * X_u, Vec * X_p, IS is_U_hat, IS is_P_hat);
double computeErrorAndCheck( Vec X_anal, Vec X_output, IS is_U, IS is_P, Vec X_u, Vec X_p);
