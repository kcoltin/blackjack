#include "stp.h"
#include <math.h>
#include <stdlib.h> 
#include <sys/time.h> 
#include <gsl/gsl_rng.h> 
#include "error.h"
#include "linal.h"
#include "moremath.h"





//--------------------------------------------------------------------------------------------------
// Seeds the runif() function with a pseudo-random starting point, and 
// initializes a random number generator.  
// This function is automatically called by runif the first time that runif is
// called in a program, so it never needs to be called by the user. 
//--------------------------------------------------------------------------------------------------
gsl_rng * init_runif ()
{
	const gsl_rng_type *type; 
	gsl_rng *rng; 
	struct timeval time; 

	gsl_rng_env_setup (); 
	type = gsl_rng_default; 
	rng = gsl_rng_alloc (type); 

	// Set seed: uses the "timeval" structure from sys/time.h to seed time based on microseconds 
	// rather than just seconds, as the traditional way of seeding srand() does. 
	gettimeofday (&time, NULL); 
	gsl_rng_set (rng, (unsigned long int) time.tv_usec * time.tv_sec); // seed the rng 

	// Note: rng is alloc-ed but never freed in either this function or in runif - it stays in
	// memory throughout the duration of the program. This isn't really a memory leak problem 
	// because there is only a single instance of it (since this function is only called 
	// once per program, through runif()). 

	return rng;  
}


//--------------------------------------------------------------------------------------------------
// Uniform random number generator. Returns a random number uniformly distributed on [0, 1). 
//--------------------------------------------------------------------------------------------------
double runif ()
{
	static int is_seeded = 0; 
	static gsl_rng *rng; 
	
	if (!(is_seeded)) { 
		rng = init_runif (); 
		is_seeded = 1; 
	}

	return gsl_rng_uniform (rng); 
}


//------------------------------------------------------------------------------
// Returns a random integer with the discrete uniform distribution, between a
// and b inclusive. 
//------------------------------------------------------------------------------
int rdiscunif (int a, int b)
{
	double x; 
	int n; 
	
	x = runif(); 
	n = (int) floor ((b - a + 1) * x + a); 
	
	return n; 
}


//------------------------------------------------------------------------------
// Returns a random draw from a vector of probabilities. The draw takes values
// in [1, N]. Returns a value of i with probability equal to the entry v[i-1]. 
//------------------------------------------------------------------------------
int randdraw (double *v, int N)
{
	double sum, x;  
	int i; 
	
	x = runif(); 
	sum = v[0];  
	i = 0; 
	while (x > sum && i < N-1) 
	{
		i++; 
		sum += v[i]; 
	}
	
	return i + 1; 
}



//------------------------------------------------------------------------------
// Returns a random draw from a vector. The ith entry of the vector is the 
// number of items of type i, out of a total of "sum" where "sum" is the sum of 
// all entries, i.e. the total number of items. The draw takes values in [1, N].
// Returns a value of i with probability equal to the entry v[i-1]/sum.  
//------------------------------------------------------------------------------
int randdraw_count (int *v, int N)
{
	int sum, x, count, i; 
	
	sum = 0; 
	for (i = 0; i < N; i++)
		sum += v[i]; 
	
	//The idea is to pick the "xth" item in the array, starting from the 
	//beginning and counting up. 
	x = rdiscunif(1, sum); 
	
	count = v[0]; 
	i = 0; 
	while (x > count && i < N - 1)
	{
		i++; 
		count += v[i]; 
	}
	
	return i + 1; 
}


//------------------------------------------------------------------------------
// Returns a random draw from a vector. The ith entry of the vector is the 
// number of items of type i, out of a total of "sum" where "sum" is the sum of 
// all entries, i.e. the total number of items. The draw takes values in [1, N].
// Returns a value of i with probability equal to the entry v[i-1]/sum.  
// 
// The difference between this and randdraw_count is that this version accepts 
// "sum" as an argument rather than computing it - this is more efficient when 
// multiple function calls are made and the value of "sum" is already known. 
//------------------------------------------------------------------------------
int randdraw_count2 (int *v, int N, int sum)
{
	int x, count, i; 
	
	//The idea is to pick the "xth" item in the array, starting from the 
	//beginning and counting up. 
	x = rdiscunif(1, sum); 
	
	count = v[0]; 
	i = 0; 
	while (x > count && i < N - 1)
	{
		i++; 
		count += v[i]; 
	}
	
	return i + 1; 
}










