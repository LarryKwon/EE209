#include "dynarray.h"
#include "token.h"
#include "lexical.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
/*--------------------------------------------------------------------*/
enum
{
   MAX_LINE_SIZE = 1024
};

enum
{
   FALSE,
   TRUE
};

/*--------------------------------------------------------------------*/

static enum TokenType ioTokenType(char c)
{
   if (!((c == '<') || (c == '>') || (c == '|')))
   {
      fprintf(stderr, "wrong parameter\n");
   }

   if (c == '<')
   {
      return TOKEN_STDIN;
   }
   else if (c == '>')
   {
      return TOKEN_STDOUT;
   }
   else if (c == '|')
   {
      return TOKEN_PIPE;
   }
}

/*--------------------------------------------------------------------*/

int lexLine(const char *pcLine, DynArray_T oTokens)

/* Lexically analyze string pcLine.  Populate oTokens with the
   tokens that pcLine contains.  Return 1 (TRUE) if successful, or
   0 (FALSE) otherwise.  In the latter case, oTokens may contain
   tokens that were discovered before the error. The caller owns the
   tokens placed in oTokens. */

/* lexLine() uses a DFA approach.  It "reads" its characters from
   pcLine. */

{
   enum LexState
   {
      STATE_START,
      STATE_IN_WORD,
      STATE_IN_STR,
      STATE_IN_OUTPUT

   };

   enum LexState eState = STATE_START;

   int iLineIndex = 0;
   int iValueIndex = 0;
   char c;
   char acValue[MAX_LINE_SIZE] = {'\0'};
   struct Token *psToken;

   assert(pcLine != NULL);
   assert(oTokens != NULL);

   for (;;)
   {
      /* "Read" the next character from pcLine. */
      c = pcLine[iLineIndex++];
      enum TokenType tokenType;

      switch (eState)
      {
      case STATE_START:
         if (isspace(c))
         {
            if ((c == '\n') || (c == '\0'))
            {
               return TRUE;
            }
            eState = STATE_START;
         }
         else
         {
            if (c == '\"')
            {
               eState = STATE_IN_STR;
            }
            else if ((c == '<') || (c == '>') || (c == '|'))
            {
               /* Create a Output token */
               acValue[iValueIndex++] = c;
               tokenType = ioTokenType(c);
               eState = STATE_IN_OUTPUT;
            }
            else
            {
               acValue[iValueIndex++] = c;
               eState = STATE_IN_WORD;
            }
         }
         break;
      case STATE_IN_WORD:
         if (isspace(c))
         {
            /* Create a WORD token. */
            acValue[iValueIndex] = '\0';
            tokenType = TOKEN_WORD;
            psToken = makeToken(tokenType, acValue);
            if (psToken == NULL)
            {
               fprintf(stderr, "Cannot allocate memory\n");
               return FALSE;
            }
            if (!DynArray_add(oTokens, psToken))
            {
               fprintf(stderr, "Cannot allocate memory\n");

               return FALSE;
            }
            iValueIndex = 0;

            if ((c == '\n') || (c == '\0'))
            {
               return TRUE;
            }
            else
            {
               eState = STATE_START;
            }
         }
         else
         {
            if (c == '\"')
            {
               eState = STATE_IN_STR;
            }
            else if ((c == '<') || (c == '>') || (c == '|'))
            {
               /* Create a Word token */
               acValue[iValueIndex] = '\0';
               tokenType = TOKEN_WORD;
               psToken = makeToken(tokenType, acValue);
               if (psToken == NULL)
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               if (!DynArray_add(oTokens, psToken))
               {
                  fprintf(stderr, "Cannot allocate memory\n");
                  return FALSE;
               }
               iValueIndex = 0;

               acValue[iValueIndex++] = c;
               tokenType = ioTokenType(c);
               eState = STATE_IN_OUTPUT;
            }
            else
            {
               acValue[iValueIndex++] = c;
               eState = STATE_IN_WORD;
            }
         }
         break;

      case STATE_IN_STR:
         if (c == '\"')
         {
            eState = STATE_IN_WORD;
         }
         else if ((c == '\n') || (c == '\0'))
         {
            fprintf(stderr, "ERROR - unmatched quote\n");
            return FALSE;
         }
         else
         {
            acValue[iValueIndex++] = c;
            eState = STATE_IN_STR;
         }
         break;

      case STATE_IN_OUTPUT:

         /* Create a Output token */
         acValue[iValueIndex] = '\0';

         psToken = makeToken(tokenType, acValue);
         if (psToken == NULL)
         {
            fprintf(stderr, "Cannot allocate memory\n");
            return FALSE;
         }
         if (!DynArray_add(oTokens, psToken))
         {
            fprintf(stderr, "Cannot allocate memory\n");
            return FALSE;
         }
         iValueIndex = 0;

         if (isspace(c))
         {
            if ((c == '\n') || (c == '\0'))
            {
               return TRUE;
            }
            eState = STATE_START;
         }
         else
         {
            if (c == '\"')
            {
               eState = STATE_IN_STR;
            }
            else
            {
               acValue[iValueIndex++] = c;
               eState = STATE_IN_WORD;
            }
         }
         break;
      default:
         assert(FALSE);
      }
   }
}

/*--------------------------------------------------------------------

int main(void)

// Read a line from stdin, and write to stdout each number and word
// that it contains.  Repeat until EOF.  Return 0 iff successful.

{
   char acLine[MAX_LINE_SIZE];
   DynArray_T oTokens;
   int iSuccessful;

   printf("------------------------------------\n");
   while (fgets(acLine, MAX_LINE_SIZE, stdin) != NULL)
   {
      oTokens = DynArray_new(0);
      if (oTokens == NULL)
      {
         fprintf(stderr, "Cannot allocate memory\n");
         exit(EXIT_FAILURE);
      }

      iSuccessful = lexLine(acLine, oTokens);
      if (iSuccessful)
      {
         // printf("Numbers:  ");
         // DynArray_map(oTokens, printNumberToken, NULL);
         // printf("\n");

         // printf("Words:  ");
         // DynArray_map(oTokens, printWordToken, NULL);
         // printf("\n");
         DynArray_map(oTokens, printAnyToken, NULL);
         printf("\n");
      }
      printf("------------------------------------\n");

      DynArray_map(oTokens, freeToken, NULL);
      DynArray_free(oTokens);
   }

   return 0;
}
*/
