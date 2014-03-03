/*
 * morestrings.h
 * Kevin Coltin  
 *
 * Contains functions for string manipulation beyond those included in the 
 * standard string.h. 
 *
 */

#ifndef MORESTRINGS_H 
#define MORESTRINGS_H 

int endsWith (const char *str, const char *end); 
int endsWithIgnoreCase (const char *str, const char *end); 
void stringReplace (char *str, char oldch, char newch); 
char tolowercase (char c); 


#endif 




