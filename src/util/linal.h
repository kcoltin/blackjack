/* 
linal.h 
Kevin Coltin 

File with various linear algebra functions. This version has been substantially
changed by me - the original version that I got from Jeremiah Jones is in the
file "linal_orig.h". 
*/


#ifndef LINAL_H
#define LINAL_H  

#include <stdio.h>
#include <complex.h> 
#include <gsl/gsl_math.h> 
#include <gsl/gsl_eigen.h> 

double dot(double *x, double *y, int N);
double complex rdotc(double *x, double complex *y, int N); 
double complex cdotc(double complex *x, double complex *y, int N); 
void rowswap(double **A, int i1, int i2);
void c_rowswap(double complex **A, int i1, int i2);
double * mtimesv(double **A, double *x, int M, int N);
double * vtimesm(double *x, double **A, int M, int N);
double ** mtimesm(double **A, double **B, int M, int K, int N);
double complex * mtimescv (double **A, double complex *x, int M, int N); 
double complex ** cmtimescm (double complex **A, double complex **B, int M, int K, int N); 
double ** cmtimescm_real (double complex **A, double complex **B, int M, int K, int N); 
double norm(double *x, int N);
double c_m2norm (double complex **X, int N); 
double ** inverse(double **A, int N);
double complex ** c_inverse(double complex **A, int N);
double det(double **A, int N);
double complex c_det(double complex **A, int N);
int is_csingular (double complex **A, int N); 
double ccondit_num (double complex **A, int N); 
double trace(double **A, int N);
void kron(double **A, double **B, double **C, int m, int n, int p, int q);
double * solve(double **A, double *b, int N);
double complex * c_solve(double complex **A, double complex *b, int N);
double complex ** c_msolve (double complex **A, double complex **B, int N, int M); 
double ** transpose(double **A, int M, int N);
double complex ** c_transpose(double complex **A, int M, int N);
double complex ** conjtransp (double complex **A, int M, int N); 
double complex ** conjugate (double complex **A, int M, int N); 
int pivot_row(double **A, int M, int N, int start_row, int column);
int c_pivot_row(double complex **A, int M, int N, int start_row, int column);
double ** zerosm(int M, int N); 
double * zerosv(int N);
double complex ** c_zerosm(int M, int N); 
double complex * c_zerosv(int N);  
double ** eye(int N);
double complex ** c_eye(int N);
double ** D2(int N);
double * linspace(double, double, int); 
void printv(double *x, int N, int by_column, int equal_width);
void printm(double **A, int M, int N, int equal_width);
void fprintv(FILE *stream, double *x, int N, int by_column, int equal_width);
void fprintm(FILE *stream, double **A, int M, int N, int equal_width);
void printcv(double complex *x, int N, int by_column, int equal_width);
void printcm(double complex **A, int M, int N, int equal_width); 
double ** readmatrix(FILE *stream, int M, int N); 
double * matrixmax (double**, int, int); 
double * matrixmin (double**, int, int); 
double * matrixmean (double**, int, int); 
double matrixMedianEntrywise (double **A, int m, int n);
double matrixMedianEntrywiseIgNan (double **A, int m, int n);
double vectormax (double *, int); 
double vectormaxabs (double *, int); 
double vectormin (double *, int); 
double vectormean (double *, int); 
double vectorsum (double *, int); 
double complex c_vectormax (double complex *, int); 
double complex c_vectormin (double complex *, int); 
double * allocvector (int); 
double ** allocmatrix (int, int); 
double complex ** c_allocmatrix (int, int); 
double complex * c_allocvector (int); 
void freematrix(double **, int); 
void freecmatrix(double complex **, int); 
void eigendecomp (double **A, double complex **X, double complex *d, int n);
double ** matrixpow (double **, int, double); 
double ** matrixpow_basic (double **, int, int); 
double ** matrixpow_circstoch (double **, int, double); 
double * copyv (double *, int); 
double ** copym (double **, int, int); 
void copycv_inplace (double complex *copy, double complex *orig, int n); 
void copycm_inplace (double complex **copy, double complex **orig, int m, int n); 
double * diffs (double *, int); 
double maxdiff (double **, double **, int, int);
void flipVector (double *v, int N);
void flipLR (double **, int, int); 
double ** slicem (double **A, int i1, int i2, int j1, int j2);
double * matrixToVector (double **A, int m, int n); 
double complex * cmatrixToCVector (double complex **A, int m, int n); 
double * pctChanges (double **A, int numsims, int numsteps); 
double ** pctChangesMatrix (double **A, int numsims, int numsteps); 
double complex * c_eigvals (double complex **A, int N); 
double complex * c_singVals (double complex **A, int M, int N); 
double ** realPartOfMat (double complex **A, int M, int N); 
double ** imagPartOfMat (double complex **A, int M, int N); 
double complex ** gslmToCx (gsl_matrix_complex *A, int M, int N); 
double complex * gslvToCx (gsl_vector_complex *x, int N); 
double complex * cmat_to_fortran (double complex **A, int M, int N); 


#endif 
