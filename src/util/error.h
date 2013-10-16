/*
 * error.h
 * Kevin Coltin 
 *
 * Contains several functions for throwing errors that may arise from memory
 * allocation, I/O, etc. 
 */

#ifndef ERROR_H
#define ERROR_H 

void throwErr (const char *, const char *); 
void throwMemErr (const char *, const char *); 
void throwFileErr (const char *, const char *); 
void warning (const char *, const char *); 

#endif 
