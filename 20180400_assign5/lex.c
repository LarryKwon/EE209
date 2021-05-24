/* 원윤재 Assignment 5 lex.c */

#include "lex.h"
#include "dynarray.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*-------------------------------------------------------------------*/
/* A Token is either a redirectional(special) character or a word,
   expressed as a string. */
struct Token
{
    enum TokenType eType;
    /* The type of the token. */

    char *pcValue;
    /* The string which is the token's value. */
};

/*-------------------------------------------------------------------*/
/* Return the pcValue of Token */
char * getpcValueToken(struct Token * Token){
    return Token->pcValue;}
/*-------------------------------------------------------------------*/
// Return the type of Token
enum TokenType geteTypeToken(struct Token *Token){return Token->eType;}
/*-------------------------------------------------------------------*/
// Free Token pvItem. pvExtra is unused.
void freeToken(void *pvItem, void *pvExtra)
{
    struct Token *psToken = (struct Token*)pvItem;
    free(psToken->pcValue);
    free(psToken);
}
/*-------------------------------------------------------------------*/
// Print token pvItem to stdout iff it is a redirectional character.
// pvExtra is unused.
void printRedcharToken(void *pvItem, void *pvExtra)
{
    struct Token *psToken = (struct Token*)pvItem;
    if (psToken->eType == TOKEN_REDCHAR)
        printf("%s\n", psToken->pcValue);
}
/*-------------------------------------------------------------------*/
/* Print token pvItem to stdout iff it is a word.  pvExtra is
   unused. */
void printWordToken(void *pvItem, void *pvExtra)
{
    struct Token *psToken = (struct Token*)pvItem;
    if (psToken->eType == TOKEN_WORD)
        printf("%s\n", psToken->pcValue);
}
/*-------------------------------------------------------------------*/
/* Print token pvItem.  pvExtra is
   unused. */

void printAllToken(void *pvItem, void *pvExtra)
{
    struct Token *psToken = (struct Token*)pvItem;
    printf("%s\n", psToken->pcValue);
}
/*-------------------------------------------------------------------*/
/* Create and return a Token whose type is eTokenType and whose
       value consists of string pcValue.  Return NULL if insufficient
       memory is available.  The caller owns the Token. */

struct Token *makeToken(enum TokenType eTokenType,
        char *pcValue)
{
    struct Token *psToken;

    psToken = (struct Token*)malloc(sizeof(struct Token));
    if (psToken == NULL)
        return NULL;

    psToken->eType = eTokenType;

    psToken->pcValue = (char*)malloc(strlen(pcValue) + 1);
    if (psToken->pcValue == NULL)
    {
        free(psToken);
        return NULL;
    }

    strcpy(psToken->pcValue, pcValue);

    return psToken;
}
/*-------------------------------------------------------------------*/
// Check if character c is a redirectional(special) character.
int isredchar(char c,void *pvExtra)
{return ((c=='>')||(c=='<')||(c=='|'));}
/*-------------------------------------------------------------------*/
// Reinitialize the elements of character array acValue with null char
void reinitializearray(char *acValue){
    for(int i=0;i<MAX_LINE_SIZE;i++){
        acValue[i]='\0';
    }
}
/*-------------------------------------------------------------------*/
/* Lexically analyze string pcLine.  Populate oTokens with the
   tokens that pcLine contains.  Return 1 (TRUE) if successful, or
   0 (FALSE) otherwise.  In the latter case, oTokens may contain
   tokens that were discovered before the error. The caller owns the
   tokens placed in oTokens. */

/* lexLine() uses a DFA approach.  It "reads" its characters from
   pcLine. */

int lexLine(const char *pcLine, DynArray_T oTokens, char**argv)
{
    enum LexState {STATE_SPACE, STATE_NONSPACE,
        STATE_REDCHAR,STATE_QUOTE,STATE_ERROR,STATE_EXIT};

    enum LexState eState = STATE_SPACE;

    int iLineIndex = 0;
    int iValueIndex = 0;
    char c;
    char acValue[MAX_LINE_SIZE]={'\0'};
    struct Token *psToken;

    assert(pcLine != NULL);
    assert(oTokens != NULL);

    for (;;)
    {
        /* "Read" the next character from pcLine. */
        c = pcLine[iLineIndex++];

        switch (eState)
        {
            case STATE_SPACE:
                if ((c == '\n') || (c == '\0'))
                    return TRUE;
                else if (isspace(c))
                {
                    eState = STATE_SPACE;
                }
                else if (c=='"') eState = STATE_QUOTE;
                else if (isredchar(c,NULL)){
                    acValue[iValueIndex++]=c;
                    eState=STATE_REDCHAR;
                }
                else if (!isspace(c))
                {
                    acValue[iValueIndex++] = c;
                    eState = STATE_NONSPACE;
                } 

                else
                {
                    fprintf(stderr, "%s: Invalid line\n",argv[0]);
                    return FALSE;
                }
                break;

            case STATE_NONSPACE:
                if ((c == '\n') || (c == '\0'))
                {
                    if (iValueIndex){
                        /* Create a Word token. */
                        acValue[iValueIndex] = '\0';
                        psToken = makeToken(TOKEN_WORD, acValue);
                        if (psToken == NULL)
                        {
                            fprintf(stderr,"Cannot allocate memory\n");
                            return FALSE;
                        }
                        if (! DynArray_add(oTokens, psToken))
                        {
                            fprintf(stderr,"Cannot allocate memory\n");
                            return FALSE;
                        }
                    }
                    iValueIndex = 0;
                    reinitializearray(acValue);
                    return TRUE;
                }
                else if (isspace(c))
                {
                    /* Create a Word token. */
                    if (iValueIndex){
                        acValue[iValueIndex] = '\0';
                        psToken = makeToken(TOKEN_WORD, acValue);
                        if (psToken == NULL)
                        {
                            fprintf(stderr,"Cannot allocate memory\n");
                            return FALSE;
                        }
                        if (! DynArray_add(oTokens, psToken))
                        {
                            fprintf(stderr,"Cannot allocate memory\n");
                            return FALSE;
                        }
                    }
                    iValueIndex = 0;
                    reinitializearray(acValue);
                    eState = STATE_SPACE;
                }
                else if (isredchar(c,NULL))
                {
                    /* Create a Word token. */
                    if(iValueIndex){
                        acValue[iValueIndex] = '\0';
                        psToken = makeToken(TOKEN_WORD, acValue);
                        if (psToken == NULL)
                        {
                            fprintf(stderr,"Cannot allocate memory\n");
                            return FALSE;
                        }
                        if (! DynArray_add(oTokens, psToken))
                        {
                            fprintf(stderr,"Cannot allocate memory\n");
                            return FALSE;
                        }
                    }
                    iValueIndex = 0;
                    reinitializearray(acValue);
                    acValue[iValueIndex++]=c;
                    eState = STATE_REDCHAR;
                }
                else if (c=='"') eState=STATE_QUOTE;
                else if (!isspace(c))
                {
                    acValue[iValueIndex++]=c;
                    eState=STATE_NONSPACE;
                }

                else
                {
                    fprintf(stderr, "Invalid line\n");
                    return FALSE;
                }
                break;

            case STATE_REDCHAR:
                if ((c == '\n') || (c == '\0'))
                {
                    /* Create a Redchar token. */
                    acValue[iValueIndex] = '\0';
                    psToken = makeToken(TOKEN_REDCHAR, acValue);
                    if (psToken == NULL)
                    {
                        fprintf(stderr, "Cannot allocate memory\n");
                        return FALSE;
                    }
                    if (! DynArray_add(oTokens, psToken))
                    {
                        fprintf(stderr, "Cannot allocate memory\n");
                        return FALSE;
                    }
                    iValueIndex = 0;
                    reinitializearray(acValue);
                    return TRUE;
                }
                else if (isredchar(c,NULL))
                {
                    // Create a Redchar token.
                    acValue[iValueIndex] = '\0';
                    psToken = makeToken(TOKEN_REDCHAR, acValue);
                    if (psToken == NULL)
                    {
                        fprintf(stderr, "Cannot allocate memory\n");
                        return FALSE;
                    }
                    if (! DynArray_add(oTokens, psToken))
                    {
                        fprintf(stderr, "Cannot allocate memory\n");
                        return FALSE;
                    }
                    iValueIndex = 0;
                    reinitializearray(acValue);

                    acValue[iValueIndex++]=c;
                    eState = STATE_REDCHAR;
                }
                else if (isspace(c))
                {
                    /* Create a Redchar token. */
                    acValue[iValueIndex] = '\0';
                    psToken = makeToken(TOKEN_REDCHAR, acValue);
                    if (psToken == NULL)
                    {
                        fprintf(stderr, "Cannot allocate memory\n");
                        return FALSE;
                    }
                    if (! DynArray_add(oTokens, psToken))
                    {
                        fprintf(stderr, "Cannot allocate memory\n");
                        return FALSE;
                    }
                    iValueIndex = 0;
                    reinitializearray(acValue);

                    eState = STATE_SPACE;
                }
                else if (c=='"')
                {
                    /* Create a Redchar token. */
                    acValue[iValueIndex] = '\0';
                    psToken = makeToken(TOKEN_REDCHAR, acValue);
                    if (psToken == NULL)
                    {
                        fprintf(stderr, "Cannot allocate memory\n");
                        return FALSE;
                    }
                    if (! DynArray_add(oTokens, psToken))
                    {
                        fprintf(stderr, "Cannot allocate memory\n");
                        return FALSE;
                    }
                    iValueIndex = 0;

                    eState = STATE_QUOTE;
                }
                else if (!isspace(c))
                {
                    /* Create a Redchar token. */
                    acValue[iValueIndex] = '\0';
                    psToken = makeToken(TOKEN_REDCHAR, acValue);
                    if (psToken == NULL)
                    {
                        fprintf(stderr, "Cannot allocate memory\n");
                        return FALSE;
                    }
                    if (! DynArray_add(oTokens, psToken))
                    {
                        fprintf(stderr, "Cannot allocate memory\n");
                        return FALSE;
                    }
                    iValueIndex = 0;
                    reinitializearray(acValue);

                    acValue[iValueIndex++]=c;

                    eState = STATE_NONSPACE;
                }

                else
                {
                    fprintf(stderr, "Invalid line\n");
                    return FALSE;
                }
                break;
            case STATE_QUOTE:
                if((c=='\n')||(c=='\0'))
                {
             fprintf(stderr,"%s: Could not find quote pair\n",argv[0]);
                    return FALSE;
                }
                else if (c=='"') //checks for empty double quote pair
                {
                   if (((iLineIndex==2)&&(pcLine[iLineIndex-2]=='"'))||
       ((pcLine[iLineIndex-2]=='"')&&((isspace(pcLine[iLineIndex-3]))||
                       (isredchar(pcLine[iLineIndex-3],NULL))))){
          if ((pcLine[iLineIndex]=='\n')||(pcLine[iLineIndex]=='\0')){
                            //Create empty word token.
                            acValue[0]='\0';
                            psToken=makeToken(TOKEN_WORD,acValue);
                            DynArray_add(oTokens,psToken);
                            return TRUE;

                        }
                        else if (isspace(pcLine[iLineIndex])) {
                            //Create empty word token.
                            acValue[0]='\0';
                            psToken=makeToken(TOKEN_WORD,acValue);
                            DynArray_add(oTokens,psToken);

                            eState=STATE_SPACE;
                            iLineIndex++;

                        }
                        else if (isredchar(pcLine[iLineIndex],NULL)) {
                            //Create empty word token.
                            acValue[0]='\0';
                            psToken=makeToken(TOKEN_WORD,acValue);
                            DynArray_add(oTokens,psToken);
                            
                            acValue[iValueIndex++]=pcLine[iLineIndex];
                            eState=STATE_REDCHAR;
                            iLineIndex++;

                        }

                        else if(pcLine[iLineIndex]=='"'){
                            int tempIndex=iLineIndex;
                            while(pcLine[iLineIndex++]=='"');
                            iLineIndex--;
            if((pcLine[iLineIndex]=='\n')||(pcLine[iLineIndex]=='\0')){
                                if ((iLineIndex-tempIndex)%2==1){
             fprintf(stderr,"%s: Could not find quote pair\n",argv[0]);
                                    return FALSE;
                                }
                                //Create empty word token.
                                acValue[0]='\0';
                                psToken=makeToken(TOKEN_WORD,acValue);
                                DynArray_add(oTokens,psToken);
                                return TRUE;
                            }
                            else if ((iLineIndex-tempIndex)%2==1){
                                    assert(iValueIndex==0);
                             acValue[iValueIndex++]=pcLine[iLineIndex];
                                    eState=STATE_QUOTE;
                                    iLineIndex++;
                                }
                            else if(isspace(pcLine[iLineIndex])){
                                //Create empty word token.
                                acValue[0]='\0';
                                psToken=makeToken(TOKEN_WORD,acValue);
                                DynArray_add(oTokens,psToken);
                                eState=STATE_SPACE;
                                iLineIndex++;
                            }
                        else if (isredchar(pcLine[iLineIndex],NULL)) {
                            //Create empty word token
                            acValue[0]='\0';
                            psToken=makeToken(TOKEN_WORD,acValue);
                            DynArray_add(oTokens,psToken);
                            acValue[iValueIndex++]=pcLine[iLineIndex];
                            eState=STATE_REDCHAR;
                            iLineIndex++;

                        }
                            else eState=STATE_NONSPACE;
                        }
                        else eState=STATE_NONSPACE;
                    }
                    else eState=STATE_NONSPACE;
                }
                else {
                    acValue[iValueIndex++]=c;
                    eState=STATE_QUOTE;
                }
                break;

            default: //should not reach this point
                assert(FALSE);
        }
    }
}
/*-------------------------------------------------------------------*/
