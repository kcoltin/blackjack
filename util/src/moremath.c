#include "moremath.h" 
#include <math.h> 
#include <stdlib.h> 
#include "boolean.h" 
#include "error.h"

//------------------------------------------------------------------------------
// Max of two values 
//------------------------------------------------------------------------------
int maxi (int n, int m)
{
	return n >= m ? n : m; 
}



//--------------------------------------------------------------------------------------------------
// Returns an approximation of the machine epsilon for double precision numbers.  
//--------------------------------------------------------------------------------------------------
double machine_eps () 
{
	double eps = 1.; 

	do {
		eps /= 2.; 
	}
	while (1. + eps / 2. > 1.); 

	return eps; 
}













