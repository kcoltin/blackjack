/*
 * fourier.h
 * Kevin Coltin  
 *
 * Functions for Fourier analysis and signal processing. 
 */

#ifndef FOURIER_H 
#define FOURIER_H 

#include <complex.h>

double complex * real_fft (double *u, int n); 
void fft (double *data, int nn); 
double complex * dft (double complex *u, int n); 
double complex * real_dft (double *u, int n); 
double ** periodogram (double *u, int N, double T); 
int isMaxInNeighborhood (int i, int radius, double **pgram, int n);  


#endif

