
/*--------------------------------------------------------------------*/
/* token.c                                                            */
/*--------------------------------------------------------------------*/

#include "token.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
struct Token
{
   enum TokenType eType;
   /* The type of the token. */

   char *pcValue;
   /* The string which is the token's value. */
};

/*--------------------------------------------------------------------*/

void freeToken(void *pvItem, void *pvExtra)
{
   struct Token *psToken = (struct Token *)pvItem;
   free(psToken->pcValue);
   free(psToken);
}

/*--------------------------------------------------------------------*/

void printNumberToken(void *pvItem, void *pvExtra)
{
   struct Token *psToken = (struct Token *)pvItem;
   if (psToken->eType == TOKEN_NUMBER)
      printf("%s ", psToken->pcValue);
}

/*--------------------------------------------------------------------*/

void printWordToken(void *pvItem, void *pvExtra)
{
   struct Token *psToken = (struct Token *)pvItem;
   if (psToken->eType == TOKEN_WORD)
      printf("%s ", psToken->pcValue);
}

/*--------------------------------------------------------------------*/

void printStringToken(void *pvItem, void *pvExtra)
{
   struct Token *psToken = (struct Token *)pvItem;
   if (psToken->eType == TOKEN_STRING)
      printf("%s ", psToken->pcValue);
}

/*--------------------------------------------------------------------*/
void printAnyToken(void *pvItem, void *pvExtra)
{
   struct Token *psToken = (struct Token *)pvItem;
   printf("%d : %s \n", psToken->eType, psToken->pcValue);
}

/*--------------------------------------------------------------------*/

struct Token *makeToken(enum TokenType eTokenType,
                        char *pcValue)
{
   struct Token *psToken;

   psToken = (struct Token *)malloc(sizeof(struct Token));
   if (psToken == NULL)
      return NULL;

   psToken->eType = eTokenType;

   psToken->pcValue = (char *)malloc(strlen(pcValue) + 1);
   if (psToken->pcValue == NULL)
   {
      free(psToken);
      return NULL;
   }

   strcpy(psToken->pcValue, pcValue);

   return psToken;
}