#include "linal.h" 
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <complex.h> 
#include "moremath.h"
#include "boolean.h" 
#include "error.h"
#include <gsl/gsl_math.h> 
#include <gsl/gsl_eigen.h> 

double dot (double *x, double *y, int N)
{
	int i;
	double d = 0.;
	for(i=0; i < N; i++)
		d += x[i]*y[i];

	return d;
}


void c_rowswap(double complex **A, int i1, int i2)
{
	double complex *t = A[i1];
	A[i1] = A[i2];
	A[i2] = t;
}




// Product xA of a M x N matrix A and a row vector x of length M
double * vtimesm(double *x, double **A, int M, int N)
{
	int j;
	double *b = NULL; 
	double **At = transpose(A, M, N); 
	
	b = (double *) malloc (N * sizeof(double)); 

	for(j = 0; j < N; j++)
		b[j] = dot(x, At[j], M);

	freematrix(At, M); 
	return b; 
}


//------------------------------------------------------------------------------
// Product of an M x K matrix A and a K x N matrix B. 
//------------------------------------------------------------------------------
double ** mtimesm(double **A, double **B, int M, int K, int N)
{
	double sum = 0.;
	int i, j, k;
	double **C; 
	
	C = allocmatrix(M, N); 
	if (C == NULL)
		throwMemErr("C", "mtimesm"); 
	
	for(i = 0; i < M; i++)
	{
		for(j = 0; j < N; j++)
		{
			for(k = 0; k < K; k++)
			{
				sum += A[i][k]*B[k][j];
			}
			C[i][j] = sum;
			sum = 0.;
		}
	}
	
	return C; 
}



// Indicates whether a complex matrix is numerically almost singular. 
// The matrix is defined to be numerically singular if the condition number
// is greater than 1 over the machine precision. 
int is_csingular (double complex **A, int N) 
{
	double kappa = ccondit_num (A, N); //condition number 
	double eps = machine_eps (); //machine precision 

	return (kappa * eps >= 1.); 
}


// Computes the condition number of complex N x N matrix A. 
// The condition number is defined as the ratio of the largest to the smallest
// singular values. Uses LAPACK. 
// Note: may need to append _ to name of LAPACK functions.  
double ccondit_num (double complex **A, int N)
{
	double complex *a, *cwork = NULL; 
	double kappa, rcond, anorm; 
	double *rwork = NULL; 
	char NORM = '1'; 
	int info; 

	// Convert A for LAPACK functions
	a = cmat_to_fortran (A, N, N); 

	// Allocate work, rwork for zgecon (work is not used in anorm with NORM='1') 
	cwork = c_allocvector (2 * N); 
	if (cwork == NULL) throwMemErr ("cwork", "ccondit_num"); 
	rwork = allocvector (2 * N); 
	if (rwork == NULL) throwMemErr ("rwork", "ccondit_num"); 

	// Compute 1-norm of A 
	anorm = zlange_ (&NORM, &N, &N, a, &N, rwork); 
	
	// Compute reciprocal of condition number 
	zgecon_ (&NORM, &N, a, &N, &anorm, &rcond, cwork, rwork, &info); 
	kappa = 1. / rcond; 

	if (info != 0) 
		throwErr ("Illegal argument to zgecon", "ccondit_num"); 

	free (a); 
	free (cwork); 
	free (rwork); 

	return kappa; 
}




//Solves the linear system Ax = b and returns x, where A and b are known and A 
//is square. A and b are complex. 
double complex * c_solve(double complex **A, double complex *b, int N)
{
	int i, j, k, p;
	double complex m, sum = 0., temp, *x, **B;
	
	B = c_allocmatrix(N, N); 
	x = (double complex *) malloc (N * sizeof(double complex)); 
	
	for(i = 0; i < N; i++)
	{
		x[i] = b[i];

		for(j = 0; j < N; j++)
		{
			B[i][j] = A[i][j];
		}
	}

	for(j = 0; j<N-1; j++)
	{
		p = c_pivot_row(B, N, N, j, j);
		c_rowswap(B, p, j);
		temp = x[p];
		x[p] = x[j];
		x[j] = temp;
		for(i = j+1; i<N; i++)
		{
			if (B[j][j] == 0.) 
				throwErr("Singular Matrix", "c_solve");
			if (B[i][j] != 0.)
			{
			m = B[i][j] / B[j][j]; 
			m *= -1.; 
			
			//B[i] += m * B[j]
			for (k = 0; k < N; k++)
				B[i][k] += m * B[j][k]; 
			
			x[i] += m * x[j]; 
			}
		}
	}

	x[N-1] /= B[N-1][N-1]; 

	for(i=N-2;i>=0; i--)
	{
		for(j=i+1; j<N; j++)
		sum += B[i][j] * x[j]; 

		x[i] = (x[i] - sum) / B[i][i]; 
		sum = 0.;
	}

	for (i = 0; i < N; i++)
		free(B[i]);
	free(B);
	
	return x; 
}


//------------------------------------------------------------------------------
// Solves the system AX = B, where A is N x N, B is N x M, and so X is N x M, 
// where A and B are complex. 
//------------------------------------------------------------------------------
double complex ** c_msolve (double complex **A, double complex **B, int N, int M)
{
	double complex **X, **x = NULL, **b = NULL; 
	int j; 
	
	x = c_allocmatrix (M, N); 
	if (x == NULL) throwMemErr ("x", "c_msolve"); 
	b = c_transpose (B, N, M); 

	// Solve each row of X, then transpose X. 
	// x is the transpose of x, and b is the tranpose of B. 
	for (j = 0; j < M; j++)
		x[j] = c_solve (A, b[j], N); 

	X = c_transpose (x, N, M); 
	freecmatrix (x, M); 
	freecmatrix (b, M); 
	return X; 
}


//A is m x n; its transpose is n x m. 
double ** transpose(double **A, int M, int N)
{
	int i, j;
	double **B = NULL; 
	
	B = allocmatrix(N, M); 
	if (B == NULL)
		throwMemErr("B", "transpose"); 
	
	for(i = 0; i < N; i++)
		for(j = 0; j < M; j++)
			B[i][j] = A[j][i];
	
	return B; 
}


//A is m x n; its transpose is n x m. 
double complex ** c_transpose(double complex **A, int M, int N)
{
	int i, j;
	double complex **B = NULL; 
	
	B = c_allocmatrix(N, M); 
	if (B == NULL)	throwMemErr("B", "c_transpose"); 
	
	for(i = 0; i < N; i++)
		for(j = 0; j < M; j++)
			B[i][j] = A[j][i];
	
	return B; 
}



//Returns the number of the row i, at start_row or greater, in which the 
//absolute value of entry A[i][column] is greatest. 
int c_pivot_row(double complex** A, int M, int N, int start_row, int column)
{

	double complex maxx = A[start_row][column];
	int pivot = start_row, i;
	for(i = start_row; i<M; i++){
		if(cabs(A[i][column])>cabs(maxx)){
			maxx = A[i][column];
			pivot = i;
		}
	}

	return pivot;

}



double * zerosv(int N)
{
	int i;
	double *x;

	x = allocvector(N); 
	if (x == NULL)
		throwMemErr("x", "zerosv"); 
		
	for(i = 0; i < N; i++)
		x[i] = 0.;
	
	return x; 
}

double ** zerosm(int M, int N)
{
	int i,j;
	double **A; 
	
	A = allocmatrix(M, N); 
	if (A == NULL)
		throwMemErr("A", "zerosm"); 
	
	for(i = 0; i < M; i++)
		for(j = 0; j < N; j++)
			A[i][j] = 0.;

	return A; 
}

// Matrix of all integer 1's.
int ** iones (int m, int n) {
	int i, j;
	int **A; 
	
	A = (int **) malloc(m * sizeof(int *)); 
	if (A == NULL)
		throwMemErr("A", "iones"); 
	
	for (i = 0; i < m; i++) { 
		A[i] = (int *) malloc(n * sizeof(int)); 
		if (A[i] == NULL)
			throwMemErr("A", "iones"); 
	}

	for(i = 0; i < m; i++)
		for(j = 0; j < n; j++)
			A[i][j] = 1;

	return A; 
}


//Makes I an NxN identity matrix
double ** eye(int N)
{
	int i;
	double **II; 
	
	II = zerosm(N, N); 

	for(i = 0; i < N; i++)
		II[i][i] = 1.;

	return II; 
}


//------------------------------------------------------------------------------
// Returns the sum of the entries of a vector of length n. 
//------------------------------------------------------------------------------
double vectorsum (double *v, int n)
{
	int i; 
	double sum = 0.; 
	
	for (i = 0; i < n; i++) 
		sum += v[i]; 
		
	return sum; 
}




//------------------------------------------------------------------------------
// Allocates an empty vector without initializing values to zero 
//------------------------------------------------------------------------------
double * allocvector (int N)
{
	double *x = NULL;
	x = (double *) malloc(N*sizeof(double)); 
	
	return x; 
}

//------------------------------------------------------------------------------
// Allocates an empty matrix without initializing values to zero 
//------------------------------------------------------------------------------
double ** allocmatrix(int M, int N)
{
	int i;
	double **A = NULL; 
	
	A = (double **) malloc(M * sizeof(double *)); 
	for (i = 0; i < M; i++)
		A[i] = (double *) malloc(N * sizeof(double)); 

	return A; 
}


//------------------------------------------------------------------------------
// Allocates an empty complex vector 
//------------------------------------------------------------------------------
double complex * c_allocvector(int N)
{
	double complex *x = NULL;
	x = (double complex *) malloc(N*sizeof(double complex)); 
	
	return x; 
}

//------------------------------------------------------------------------------
// Allocates an empty complex matrix 
//------------------------------------------------------------------------------
complex ** c_allocmatrix(int M, int N)
{
	int i;
	double complex **A = NULL; 
	
	A = (double complex **) malloc(M * sizeof(double complex *)); 
	for (i = 0; i < M; i++)
		A[i] = (double complex *) malloc(N * sizeof(double complex)); 

	return A; 
}


//------------------------------------------------------------------------------
// Frees the memory of a matrix with leading dimension m. 
//------------------------------------------------------------------------------
void freematrix(double **A, int m)
{
	int i; 
	
	for (i = 0; i < m; i++)
		free(A[i]); 
	free(A); 
}

//------------------------------------------------------------------------------
// Frees the memory of an integer matrix with leading dimension m. 
//------------------------------------------------------------------------------
void freeimatrix(int **A, int m)
{
	int i; 
	
	for (i = 0; i < m; i++)
		free(A[i]); 
	free(A); 
}



//------------------------------------------------------------------------------
// Frees the memory of a complex matrix with leading dimension m. 
//------------------------------------------------------------------------------
void freecmatrix(double complex **A, int m)
{
	int i; 
	
	for (i = 0; i < m; i++)
		free(A[i]); 
	free(A); 
}


//-----------------------------------------------------------------------------
// Returns the eigendecomposition A = X*D*X^-1. 
// d is the diagonal entries of D. 
// X and d must be preallocated: X should be n x n and d should be length n. 
//-----------------------------------------------------------------------------
void eigendecomp (double **A, double complex **X, double complex *d, int n)
{
	double *a; 
	double complex **Xtmp; 
	double complex *dtmp; 
	gsl_matrix_view m; 
	gsl_vector_complex *eval; 
	gsl_matrix_complex *evec; 
	gsl_eigen_nonsymmv_workspace *w; 

	// Use GSL routine to compute eigenvalues and eigenvectors 
	a = matrixToVector (A, n, n); 
	m = gsl_matrix_view_array (a, n, n); 
	eval = gsl_vector_complex_alloc (n); 
	evec = gsl_matrix_complex_alloc (n, n); 
	w = gsl_eigen_nonsymmv_alloc (n); 

	gsl_eigen_nonsymmv (&m.matrix, eval, evec, w); 
	gsl_eigen_nonsymmv_free (w); 

	// Convert from GSL to intrinsic types 
	Xtmp = gslmToCx (evec, n, n); 
	dtmp = gslvToCx (eval, n); 

	copycm_inplace (X, Xtmp, n, n); 
	copycv_inplace (d, dtmp, n); 

	freecmatrix(Xtmp, n); 
	free(a);
	free(dtmp); 
	gsl_vector_complex_free(eval); 
	gsl_matrix_complex_free(evec); 
}


//------------------------------------------------------------------------------
// Returns A^p where p is a nonnegative integer. n is the dimension of A. 
// Uses an eigendecomposition algorithm: if A = XDX^-1, then A^p = XD^pX^-1. 
//------------------------------------------------------------------------------
double ** matrixpow (double **A, int n, double p)
{
	double **Ap, **Apt; // Apt = A^p transposed  
	int i, j; 
	// d is diagonal of matrix D. XDp = X*D^p. 
	double complex **X, **XDp = NULL, **tmp, *d; 

	X = c_allocmatrix (n, n); 
	if (X == NULL) throwMemErr ("X", "matrixpow"); 
	d = c_allocvector (n); 
	if (d == NULL) throwMemErr ("d", "matrixpow"); 

	// Decompose A = X*D*X^-1 
	eigendecomp (A, X, d, n); 

	// If X is singular, this method won't work - need to use naive method. 
	if (is_csingular (X, n))
	{
		Ap = matrixpow_basic (A, n, p); 
		freecmatrix (X, n); 
		free (d); 
		return Ap; 
	}	

	// Raise eigenvalues to the power p 
	for (i = 0; i < n; i++)
		d[i] = cpow (d[i], p); 
	// Compute XDp = X*D^p 
	XDp = c_allocmatrix (n, n); 
	if (XDp == NULL) throwMemErr ("XDp", "matrixpow"); 
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			XDp[i][j] = X[i][j] * d[j]; 
		}
	}	
	// A^p = X*D^p*X^-1, so 
	// A^p*X = X*D^p. Solve this linear system for A^p. 
	// Specifically, solve X'*(A^p)' = (X*D^p)'. 
	tmp = c_transpose (X, n, n); 
	freecmatrix (X, n); 
	X = tmp; 
	tmp = c_transpose (XDp, n, n); 
	freecmatrix (XDp, n); 
	XDp = tmp; 
	tmp = c_msolve (X, XDp, n, n); // Solve for Ap 
	Apt = realPartOfMat (tmp, n, n); // Apt = (A^p)'. a priori, A^p is real.
	Ap = transpose (Apt, n, n); // Transpose to get the solution A^p 

	freecmatrix (X, n); 
	freecmatrix (XDp, n); 
	freecmatrix (tmp, n); 
	freematrix (Apt, n); 
	free (d); 

	return Ap; 
}


//------------------------------------------------------------------------------
// Returns A^p where p is a nonnegative integer. n is the dimension of A. 
// This just uses a naive algorithm. 
//------------------------------------------------------------------------------
double ** matrixpow_basic (double **A, int n, int p)
{
	double **Ap, **tmp; 
	int i;

	if (p < 0)
		throwErr ("p must be nonnegative.", "matrixpow_basic"); 
	
	Ap = eye(n); 
	for (i = 1; i <= p; i++) 
	{
		tmp = copym(Ap, n, n); 
		freematrix(Ap, n); 
		Ap = mtimesm(tmp, A, n, n, n); 
		freematrix(tmp, n); 
	}
	
	return Ap; 
}


	
//------------------------------------------------------------------------------
// Returns a copy of a vector 
//------------------------------------------------------------------------------
double * copyv (double *v, int N)
{
	double *copy = NULL; 
	int i; 
	
	copy = allocvector(N);  
	if (copy == NULL)
		throwMemErr("copy", "copyv"); 
	
	for (i = 0; i < N; i++)
		copy[i] = v[i]; 
	
	return copy; 
}
	

//------------------------------------------------------------------------------
// Returns a copy of a matrix 
//------------------------------------------------------------------------------
double ** copym (double **A, int M, int N)
{
	double **copy; 
	int i, j; 

	copy = allocmatrix(M, N); 
	if (copy == NULL) throwMemErr("copy", "copym"); 
	
	for (i = 0; i < M; i++)
		for (j = 0; j < N; j++)
			copy[i][j] = A[i][j]; 
	
	return copy; 
}


//------------------------------------------------------------------------------
// Copies vector orig in place to vector "copy", where "copy" has already been
// allocated (dimension n). 
//------------------------------------------------------------------------------
void copycv_inplace (double complex *copy, double complex *orig, int n) 
{
	int i; 
	for (i = 0; i < n; i++)
		copy[i] = orig[i]; 
}

//------------------------------------------------------------------------------
// Copies matrix orig in place to matrix "copy", where "copy" has already been
// allocated (dimension m x n). 
//------------------------------------------------------------------------------
void copycm_inplace (double complex **copy, double complex **orig, int m, int n)
{
	int i, j; 
	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++)
			copy[i][j] = orig[i][j]; 
}



//------------------------------------------------------------------------------
// Converts A into a (m x n)-length vector. The entries of A are added to the 
// vector first by row, then by column, i.e. it adds first A[0][0], A[0][1], 
// A[0][2], ... , A[1][0], A[1][1], etc. 
//------------------------------------------------------------------------------
double * matrixToVector (double **A, int m, int n)
{
	int i, j; 
	double *v = NULL; 
	v = allocvector(m * n); 
	if (v == NULL) throwMemErr("v", "matrixToVector"); 
	
	for (i = 0; i < m; i++)
		for (j = 0; j < n; j++)
			v[i*m+j] = A[i][j]; 
	
	return v; 
}



//------------------------------------------------------------------------------
// Returns the real part of the entries of a complex matrix. 
//------------------------------------------------------------------------------
double ** realPartOfMat (double complex **A, int M, int N)
{
	double **X = NULL; 
	int i, j; 
	
	X = allocmatrix(M, N); 
	if (X == NULL) throwMemErr("X", "realPartOfMat"); 
	
	for (i = 0; i < M; i++)
		for (j = 0; j < N; j++)
			X[i][j] = creal(A[i][j]); 
	
	return X; 
}


// Converts an M x N GSL matrix of type gsl_matrix_complex to a matrix of 
// complex numbers. 
double complex ** gslmToCx (gsl_matrix_complex *A, int M, int N) 
{
	double complex **B = NULL; 
	gsl_vector_complex_view ai; 
	int i; 

	B = (double complex **) malloc(M * sizeof(double complex *)); 
	if (B == NULL) throwMemErr ("B", "gslmToCx"); 

	// There's no good way to quickly convert a GSL matrix as there is for a 
	// vector (in gslvToCx), so this somewhat naive method just acts
	// vectorwise on each row. 
	for (i = 0; i < M; i++)
	{
		ai = gsl_matrix_complex_row (A, i); 
		B[i] = gslvToCx (&ai.vector, N); 
	}

	return B; 
}


// Converts an N-dimensional GSL vector of type gsl_vector_complex to a vector 
// of complex numbers. 
double complex * gslvToCx (gsl_vector_complex *x, int N) 
{
	double complex *y = NULL; 
	gsl_vector_view xr = gsl_vector_complex_real (x); 
	gsl_vector_view xi = gsl_vector_complex_imag (x); 
	int i; 

	y = c_allocvector (N); 
	if (y == NULL) throwMemErr ("y", "gslvToCx"); 

	for (i = 0; i < N; i++)
		y[i] = xr.vector.data[i] + xi.vector.data[i]*I; 

	return y; 
}

// Converts an M x N complex matrix to a vector 
double complex * cmat_to_fortran (double complex **A, int M, int N) 
{
	double complex *a = NULL; 
	int i, j; 
	
	a = c_allocvector (M * N); 
	if (a == NULL) throwMemErr ("a", "cmat_to_fortran"); 

	for (j = 0; j < N; j++) {
		for (i = 0; i < M; i++) { 
			a[j*M+i] = A[i][j]; 
		}
	}
	
	return a; 
}


// NOTES 
// 1. For some reason, malloc'ing a single bite (sizeof char) is sufficient for
//    any length of number to be read in, but you have to malloc at least 
//		something or it will give a bus error. Beats me why. 














