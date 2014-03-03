/*
 * moremath.h
 * Kevin Coltin
 *
 *	Contains various mathematical functions not included in the standard math.h
 * header file. 
 *
 */

#ifndef MOREMATH_H
#define MOREMATH_H 

extern const double PI;

int maxi (int, int); 
int mini (int, int); 
double round (double); 
int sign (double); 
double square (double); 
int choose (int, int); 
int factorial (int); 
double newtonmethod (double (*f)(double x), double (*fp)(double x), double x0);
double newtonmethodGT (double (*f)(double x), double (*fp)(double x), 
									double x0, double xmin);
int nextGreatestPow2 (int); 
int isPow (int, int); 
int oddCeil (int); 
int hasNaN (double *, int); 
int hasInf (double *, int); 
int countGE (double *, int, double); 
double sumLT (double *, int, double); 
double sumGE (double *, int, double); 
double asinQuad (double x, int quadrant); 
double floor2 (double x, double y); 
double machine_eps (); 

#endif 
