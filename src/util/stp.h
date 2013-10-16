/*
 * stp.h
 * Kevin Coltin  
 *
 * Contains several functions for statistics and probability. Borrows from the 
 * GNU Scientific Library. 
 * 
 * Note: All function names for random variates, distributions, etc. should 
 * mirror the names from corresponding R functions, for consistency. 
 */

#ifndef STP_H 
#define STP_H 

#include <gsl/gsl_rng.h>

double variance (double *x, int n, double mu); 
double moment (int k, double *x, int n, double mu); 
gsl_rng * init_runif (); 
void init_runif_basic (); 
double runif (); 
double runif_basic (); 
double rnorm (double mu, double sigma); 
int rdiscunif (int a, int b); 
double pnorm (double x, double mu, double sigma); 
double erf (double z); 
double dbinom (int k, int n, double p); 
double pbinom (int k, int n, double p); 
int randdraw (double *v, int N); 
int randdraw_count (int *v, int N); 
int randdraw_count2 (int *v, int N, int sum); 
double percentile (double *v, int N, double pct, int isSorted); 


#endif 
