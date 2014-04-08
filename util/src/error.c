#include "error.h" 
#include <stdio.h>
#include <stdlib.h>

//------------------------------------------------------------------------------
// Throws a miscellaneous error, that may have any arbitrary error message 
// passed to it. 
//------------------------------------------------------------------------------
void throwErr (const char *message, const char *functionname)
{
	printf("Error in function %s: %s\n", functionname, message); 
	exit(EXIT_FAILURE);	
}


//------------------------------------------------------------------------------
// Causes a program to exit after displaying an error message, after a failed 
// call to malloc (or realloc, etc).  
// varname: String that should contain the name of the variable that you were
//		trying to allocated memory for. This exists in order to provide more 
//		helpful error messages. 
//------------------------------------------------------------------------------
void throwMemErr (const char *varname, const char *functionname)
{
	printf("Memory allocation error: error allocating memory for variable "
			 "%s in %s.\n", varname, functionname); 
	exit(EXIT_FAILURE); 
}


//------------------------------------------------------------------------------
// Raises a warning without causing program execution to terminate. 
//------------------------------------------------------------------------------
void warning (const char *message, const char *functionname)
{	
	fprintf(stderr, "Warning in function %s: %s\n", functionname, message); 
}






