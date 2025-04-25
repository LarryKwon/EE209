
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

   enum CommandType cType;
};

/*--------------------------------------------------------------------*/

void freeToken(void *pvItem, void *pvExtra)
{
   struct Token *psToken = (struct Token *)pvItem;
   free(psToken->pcValue);
   free(psToken);
}

/*--------------------------------------------------------------------*/

void printWordToken(void *pvItem, void *pvExtra)
{
   struct Token *psToken = (struct Token *)pvItem;
   if (psToken->eType == TOKEN_WORD)
      printf("%s ", psToken->pcValue);
}

/*--------------------------------------------------------------------*/

void printAnyTokenWithTokenType(void *pvItem, void *pvExtra)
{
   struct Token *psToken = (struct Token *)pvItem;
   printf("%d : %s \n", psToken->eType, psToken->pcValue);
}

/*--------------------------------------------------------------------*/

void printAnyTokenWithCommandType(void *pvItem, void *pvExtra)
{
   struct Token *psToken = (struct Token *)pvItem;
   printf("%d : %s \n", psToken->cType, psToken->pcValue);
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

/*--------------------------------------------------------------------*/
enum TokenType getTokenType(Token_T token)
{
   enum TokenType tokenType = token->eType;
   return tokenType;
}

/*--------------------------------------------------------------------*/
enum CommandType getCommandType(Token_T token)
{
   enum CommandType cType = token->cType;
   return cType;
}

/*--------------------------------------------------------------------*/
char *getTokenValue(Token_T token)
{
   return strdup(token->pcValue);
}

/*--------------------------------------------------------------------*/
void setCommandType(Token_T token, enum CommandType commandType)
{
   token->cType = commandType;
}