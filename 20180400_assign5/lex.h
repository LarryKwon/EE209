/* 원윤재 Assignment 5 lex.h */
/*-------------------------------------------------------------------*/
#ifndef LEX_H
#define LEX_H
#include "dynarray.h"

/* A Token is either a number or a word, expressed as a string. */
struct Token;
enum TokenType {TOKEN_WORD, TOKEN_REDCHAR};
enum {MAX_LINE_SIZE =1024};
enum {FALSE,TRUE};
/*-------------------------------------------------------------------*/
/* Free token pvItem.  pvExtra is unused. */
void freeToken(void *pvItem, void *pvExtra);
/*-------------------------------------------------------------------*/
/*Print token pvItem to stdout iff it is a redirectional(special) char.
   pvExtra is unused. */
void printRedcharToken(void *pvItem, void *pvExtra);
/*-------------------------------------------------------------------*/
char * getpcValueToken(struct Token * Token);
enum TokenType geteTypeToken(struct Token * Token);
/*-------------------------------------------------------------------*/
/* Print token pvItem to stdout iff it is a word.  pvExtra is
   unused. */
void printWordToken(void *pvItem, void *pvExtra);
/*-------------------------------------------------------------------*/
/* Print token pvItem.  pvExtra is
   unused. */
void printAllToken(void *pvItem, void *pvExtra);
/*-------------------------------------------------------------------*/
/* Create and return a Token whose type is eTokenType and whose
   value consists of string pcValue.  Return NULL if insufficient
   memory is available.  The caller owns the Token. */
struct Token *makeToken(enum TokenType eTokenType,
   char *pcValue);
/*-------------------------------------------------------------------*/
// Check if character c is a redirectional(special) character.
int isredchar(char c, void *pvExtra);
/*-------------------------------------------------------------------*/
/* Lexically analyze string pcLine.  Populate oTokens with the
   tokens that pcLine contains.  Return 1 (TRUE) if successful, or
   0 (FALSE) otherwise.  In the latter case, oTokens may contain
   tokens that were discovered before the error. The caller owns the
   tokens placed in oTokens. */

/* lexLine() uses a DFA approach.  It "reads" its characters from
   pcLine. */

int lexLine(const char *pcLine, DynArray_T oTokens,char ** argv);
/*--------------------------------------------------------------------*/
#endif
