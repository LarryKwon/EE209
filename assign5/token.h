#ifndef TOKEN_INCLUDED
#define TOKEN_INCLUDED
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
enum TokenType
{
    TOKEN_NUMBER,
    TOKEN_WORD,
    TOKEN_STRING,
    TOKEN_OUTPUT
};

/* A Token is either a number or a word, expressed as a string. */
/* The type of the token. */
/* The string which is the token's value. */
typedef struct Token *Token_T;

/* Create and return a Token whose type is eTokenType and whose
   value consists of string pcValue.  Return NULL if insufficient
   memory is available.  The caller owns the Token. */
Token_T makeToken(enum TokenType eTokenType, char *pcValue);

/* Print token pvItem to stdout iff it is a word.  pvExtra is
   unused. */
void printWordToken(void *pvItem, void *pvExtra);

/* Print token pvItem to stdout iff it is a number.  pvExtra is
   unused. */
void printNumberToken(void *pvItem, void *pvExtra);

/*print token pvItem to stdout iff it is a string. pvExtra is 
    unused. */
void printStringToken(void *pvItem, void *pvExtra);

/*print token pvItem to stdout which token type is any kind of thing 
pvExtra is unused. */
void printAnyToken(void *pvItem, void *pvExtra);

/* Free token pvItem.  pvExtra is unused. */
void freeToken(void *pvItem, void *pvExtra);

#endif