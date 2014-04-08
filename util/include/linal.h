/* linal.h 
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
void c_rowswap(double complex **A, int i1, int i2);
double * vtimesm(double *x, double **A, int M, int N);
double ** mtimesm(double **A, double **B, int M, int K, int N);
int is_csingular (double complex **A, int N); 
double ccondit_num (double complex **A, int N); 
double complex * c_solve(double complex **A, double complex *b, int N);
double complex ** c_msolve (double complex **A, double complex **B, int N, int M); 
double ** transpose(double **A, int M, int N);
double complex ** c_transpose(double complex **A, int M, int N);
int c_pivot_row(double complex **A, int M, int N, int start_row, int column);
double ** zerosm(int M, int N); 
double * zerosv(int N);
int ** iones (int m, int n); 
double ** eye(int N);
double vectorsum (double *, int); 
double * allocvector (int); 
double ** allocmatrix (int, int); 
double complex ** c_allocmatrix (int, int); 
double complex * c_allocvector (int); 
void freematrix(double **, int); 
void freecmatrix(double complex **, int); 
void freeimatrix(int **, int); 
void eigendecomp (double **A, double complex **X, double complex *d, int n);
double ** matrixpow (double **, int, double); 
double ** matrixpow_basic (double **, int, int); 
double * copyv (double *, int); 
double ** copym (double **, int, int); 
void copycv_inplace (double complex *copy, double complex *orig, int n); 
void copycm_inplace (double complex **copy, double complex **orig, int m, int n); 
double * matrixToVector (double **A, int m, int n); 
double ** realPartOfMat (double complex **A, int M, int N); 
double complex ** gslmToCx (gsl_matrix_complex *A, int M, int N); 
double complex * gslvToCx (gsl_vector_complex *x, int N); 
double complex * cmat_to_fortran (double complex **A, int M, int N); 


#endif 
