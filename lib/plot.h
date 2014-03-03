/*
 * plot.h
 * Kevin Coltin  
 *
 * Functions useful when printing code for plotting graphs. 
 */

#ifndef PLOT_H 
#define PLOT_H 

#include <stdio.h>

char * makeRvector (double *, int);
char * makeRmatrix (double **, int, int); 
char * makePythonVector (double *, int);
char * makePythonMatrix (double **, int, int); 
void plotPeriodogram (FILE *file, const char *figureFilename, double **values, 
							int N); 

#endif

