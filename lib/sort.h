/*
 * sort.h
 * Kevin Coltin 
 *
 * Functions for sorting data. 
 */


#ifndef SORT_H 
#define SORT_H 

void heapSort (double *ra, int n); 
void heapSortByRow (double **ra, int m, int n, int row); 
double * allocvector(int); //Note 1


#endif 


/* Notes 

1. This is here because I was getting a "previous declaration of allocvector was here"/
	"conficting types for allocvector" warning. 
*/








