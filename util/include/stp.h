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

gsl_rng * init_runif (); 
double runif (); 
int rdiscunif (int a, int b); 
int randdraw (double *v, int N); 
int randdraw_count (int *v, int N); 
int randdraw_count2 (int *v, int N, int sum); 


#endif 
