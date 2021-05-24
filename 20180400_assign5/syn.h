/* 원윤재 Assignment 5 syn.c */
#ifndef SYN_H
#define SYN_H
#include "dynarray.h"
/*-------------------------------------------------------------------*/
/* A CToken is a command token that contains info of argument vector
   and stdin/stdout redirection(in case of '>' and '<', if needed or
   otherwise is set to NULL). stdinred and stdoutred is
   string of name of file*/
struct CToken;
/*-------------------------------------------------------------------*/
// Return the cmdarray of CToken
char ** getcmdarrayCToken(struct CToken * CToken);
/*-------------------------------------------------------------------*/
// Return the stdinred string of CToken
char * getstdinredCToken(struct CToken * CToken);
/*-------------------------------------------------------------------*/
// Return the stdoutred string of CToken
char * getstdoutredCToken(struct CToken * CToken);
/*-------------------------------------------------------------------*/
/* Free Ctoken pvItem.  pvExtra is unused. */
void freeCToken(void *pvItem, void *pvExtra);
/*-------------------------------------------------------------------*/
// Print CToken's cmdarray, stdinred, and stdoutred. pvExtra is unused
void printAllCToken(void *pvItem, void *pvExtra);
/*-------------------------------------------------------------------*/
// Create and return CToken that has cmdarray, stdinred, and stdoutred.
// Note that CToken as well as cmdarray, stdinred, stdoutred
// is owned by the caller. Return NULL if insufficient memory.
struct CToken *makeCToken
(char ** cmdarray, char *stdinred, char *stdoutred);
/*-------------------------------------------------------------------*/
// Perform Syntactic Analysis from oTokens. Populate CTokens with
// CToken. Return 1 (TRUE) if successful, or 0 (FALSE) otherwise.
// In the latter case, CTokens may contain tokens discovered before
// error. The caller owns CToken placed in CTokens
int syn(DynArray_T oTokens, DynArray_T CTokens, char ** argv);
#endif
