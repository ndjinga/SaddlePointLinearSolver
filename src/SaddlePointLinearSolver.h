#include <petscis.h>
#include <petscksp.h>

void loadPETScMat(char* file, char* mat_type, Mat * A, int size);
Mat splitPETScMatrix2x2(Mat A_input, PetscInt n_u, PetscInt n_p, Mat * M, Mat * G, Mat *D, Mat * C, IS is_U, IS is_P);
void buildRHSVectorAndBhat( Mat A_input, PetscInt n_u, PetscInt n_p, Vec * X_anal, Vec * b_input, Vec * b_input_p, Vec * b_input_u, Vec * b_hat, IS is_U, IS is_P);
void transformSaddlePointMatrix1( Mat M, Mat G, Mat D, Mat C, Mat * A_hat, Mat * Pmat, Mat * C_hat, Mat * G_hat, Mat * diag_2M, Vec * v, int n_u);
void getSolutionFromXhat(Mat G, Vec v, Vec X_hat, Vec * X_output, Vec * X_u, Vec * X_p, IS is_U_hat, IS is_P_hat);
double computeErrorAndCheck( Vec X_anal, Vec X_output, IS is_U, IS is_P, Vec X_u, Vec X_p);

