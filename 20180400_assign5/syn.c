/* 원윤재 Assignment 5 syn.c */
#include "syn.h"
#include "lex.h"
#include "dynarray.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
/*-------------------------------------------------------------------*/
/* A CToken is a command token that contains info of argument vector
   and stdin/stdout redirection(in case of '>' and '<', if needed or
   otherwise is set to NULL). stdinred and stdoutred is
   string of name of file*/
struct CToken
{
   char ** cmdarray;
   char * stdinred;
   char * stdoutred;
};

/*-------------------------------------------------------------------*/
/* Free Ctoken pvItem.  pvExtra is unused. */
void freeCToken(void *pvItem, void *pvExtra)
{
   struct CToken *psToken = (struct CToken*)pvItem;
   // free each cmdarray elements
   char **temp=psToken->cmdarray;
   for (int i=0;i<MAX_LINE_SIZE;i++){
    free(*temp++);}
   free(psToken->cmdarray);
   free(psToken->stdinred);
   free(psToken->stdoutred);
   free(psToken);
}
/*-------------------------------------------------------------------*/
// Return the cmdarray of CToken
char ** getcmdarrayCToken(struct CToken * CToken){
    return CToken->cmdarray;}
/*-------------------------------------------------------------------*/
// Return the stdinred string of CToken
char * getstdinredCToken(struct CToken * CToken){
    return CToken->stdinred;}
/*-------------------------------------------------------------------*/
// Return the stdoutred string of CToken
char * getstdoutredCToken(struct CToken * CToken){
    return CToken->stdoutred;}
/*-------------------------------------------------------------------*/
// Print CToken's cmdarray, stdinred, and stdoutred. pvExtra is unused
void printAllCToken(void *pvItem, void *pvExtra)
{
   struct CToken *psToken = (struct CToken*)pvItem;
   char **ptr=psToken->cmdarray;
   printf("COMMAND ARRAY :\n");
   while(*ptr){
       printf("%s\n",*ptr);
       ptr++;
   }
   if(psToken->stdinred) printf("STDINRED : %s\n", psToken->stdinred);
  if(psToken->stdoutred) printf("STDOUTRED : %s\n",psToken->stdoutred);
}
/*-------------------------------------------------------------------*/
// Create and return CToken that has cmdarray, stdinred, and stdoutred.
// Note that CToken as well as cmdarray, stdinred, stdoutred
// is owned by the caller. Return NULL if insufficient memory.
struct CToken *makeCToken
(char ** cmdarray, char *stdinred, char *stdoutred)
{
   struct CToken *psToken;
   //Create CToken
   psToken = (struct CToken*)malloc(sizeof(struct CToken));
   if (psToken == NULL)
      return NULL;
   //Create own the cmdarray and assign it to the CToken
   psToken->cmdarray=(char **)calloc(MAX_LINE_SIZE,sizeof(char *));
   char ** temp2=psToken->cmdarray;
   char ** temp=cmdarray;
   for(; *temp; temp++,temp2++){
       *temp2=(char *)calloc(1,strlen(*temp)+1);
       strcpy(*temp2,*temp);
   }

   //In case stdinred specified, own stdinred and assign it to CToken
   if (stdinred&&strcmp(stdinred,"BLOCK")!=0){
   psToken->stdinred = (char*)calloc(1,strlen(stdinred) + 1);
   if (psToken->stdinred == NULL)
   {
      free(psToken);
      return NULL;
   }
   strcpy(psToken->stdinred, stdinred);
   } // In case stdinred not specified, set it to NULL
   else psToken->stdinred=NULL;
   
   //In case stdoutred specified, own stdoutred and assign it to CToken
   if (stdoutred&&strcmp(stdoutred,"BLOCK")!=0){
   psToken->stdoutred = (char*)calloc(1,strlen(stdoutred) + 1);
   if (psToken->stdoutred == NULL)
   {
      free(psToken);
      return NULL;
   }
   strcpy(psToken->stdoutred, stdoutred);
   }// In case stdoutred not specified, set it to NULL
   else psToken->stdoutred=NULL;

   return psToken;
}
/*-------------------------------------------------------------------*/
// Check for multiple redirection regarding pipes
int nopipeleft(int i, DynArray_T oTokens){
    for (i+=1;i<DynArray_getLength(oTokens);i++){
        struct Token *p=DynArray_get(oTokens,i);
    if (geteTypeToken(p)==TOKEN_REDCHAR &&
            strcmp(getpcValueToken(p),"|")==0)
            return 0;
    }
    return 1;
}

/*-------------------------------------------------------------------*/
// Perform Syntactic Analysis from oTokens. Populate CTokens with
// CToken. Return 1 (TRUE) if successful, or 0 (FALSE) otherwise.
// In the latter case, CTokens may contain tokens discovered before
// error. The caller owns CToken placed in CTokens
int syn(DynArray_T oTokens, DynArray_T CTokens,char**argv){
    char * cmdarray[MAX_LINE_SIZE]={NULL};
    char * stdinred=NULL;
    char * stdoutred=NULL;
    struct Token *lextoken;
    struct CToken * psToken;
    int index=0;
    for(int i=0; i<DynArray_getLength(oTokens); i++){
        lextoken=(struct Token*)DynArray_get(oTokens,i);
        if(geteTypeToken(lextoken)==TOKEN_WORD){
            if(i==0) {
                cmdarray[index++]=getpcValueToken(lextoken);
                if(i==DynArray_getLength(oTokens)-1){
                // create and add CToken to CTokens
                    psToken=makeCToken(cmdarray,stdinred,stdoutred);
                    if(psToken==NULL){
                        fprintf(stderr,"Cannot allocate memory\n");
                        return FALSE;
                    }
                    if(! DynArray_add(CTokens, psToken)){
                        fprintf(stderr,"Cannot allocate memory\n");
                        return FALSE;
                    }
                    return TRUE;
                }
            }
            else if(geteTypeToken(DynArray_get(oTokens,i-1))
                    ==TOKEN_REDCHAR){
        if (strcmp(getpcValueToken(DynArray_get(oTokens,i-1)),">")==0){
             if(stdoutred!=NULL){
        fprintf(stderr,"%s: Multiple redirection of standard in/out\n",
        argv[0]);
                 return FALSE;
             }
             stdoutred=getpcValueToken(lextoken);}
   else if (strcmp(getpcValueToken(DynArray_get(oTokens,i-1)),"<")==0){
                    if (stdinred!=NULL){
        fprintf(stderr,"%s: Multiple redirection of standard in/out\n",
                argv[0]);
                        return FALSE;
                    }
                    stdinred=getpcValueToken(lextoken);
                }
   else if (strcmp(getpcValueToken(DynArray_get(oTokens,i-1)),"|")==0){
                    cmdarray[index++]=getpcValueToken(lextoken);
                }

                if(i==DynArray_getLength(oTokens)-1){
                    // create and add CToken to CTokens
                    psToken=makeCToken(cmdarray,stdinred,stdoutred);
                    if(psToken==NULL){
                      fprintf(stderr,"Cannot allocate memory\n");
                       return FALSE;
                    }
                    if(! DynArray_add(CTokens, psToken)){
                        fprintf(stderr,"Cannot allocate memory\n");
                        return FALSE;
                    }
                    return TRUE;
                }
            }
            else if(i==DynArray_getLength(oTokens)-1){
                cmdarray[index++]=getpcValueToken(lextoken);
                // create and add CToken to CTokens
                psToken=makeCToken(cmdarray,stdinred,stdoutred);
                if(psToken==NULL){
                    fprintf(stderr,"Cannot allocate memory\n");
                    return FALSE;
                }
                if(! DynArray_add(CTokens, psToken)){
                    fprintf(stderr,"Cannot allocate memory\n");
                    return FALSE;
                }
                return TRUE;
            }
            else cmdarray[index++]=getpcValueToken(lextoken);
        }
        else if(geteTypeToken(lextoken)==TOKEN_REDCHAR){
            if(i==0){
                fprintf(stderr,"%s: Missing command name\n",argv[0]);
                return FALSE;
            }
            else if((i==DynArray_getLength(oTokens)-1)||
            (geteTypeToken(DynArray_get(oTokens,i+1))==TOKEN_REDCHAR)){
            fprintf(stderr,
      "%s: Pipe or redirection destination not specified\n",argv[0]);
                return FALSE;
            }
            if (strcmp(getpcValueToken(lextoken),"|")==0){
                
                // create and add CToken to CTokens
                psToken=makeCToken(cmdarray,stdinred,stdoutred);
                if(psToken==NULL){
                    fprintf(stderr,"Cannot allocate memory\n");
                    return FALSE;
                }
                if(! DynArray_add(CTokens, psToken)){
                    fprintf(stderr,"Cannot allocate memory\n");
                    return FALSE;
                }
                index=0;
                // Reinitialize command array
                for(int i=0;i<MAX_LINE_SIZE;i++){
                    cmdarray[i]=NULL;}
                if(stdoutred&&strcmp(stdoutred,"BLOCK")!=0){
                    fprintf(stderr,
              "%s: Multiple redirection of standard in/out\n",argv[0]);
                    return FALSE;
                }
                if (!nopipeleft(i,oTokens)){
                stdinred="BLOCK";
                stdoutred="BLOCK";
                }
                else{
                    stdinred="BLOCK";
                    stdoutred=NULL;
                }
            }
        }
    }
    return FALSE;
}
/*-------------------------------------------------------------------*/
