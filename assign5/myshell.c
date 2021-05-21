#include "lexical.h"
#include "syntatic.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
enum
{
    MAX_LINE_SIZE = 1024
};
static DynArray_T executionInit(DynArray_T oTokens, char *acLine)
{
    int isSuccessful;
    int valid;

    oTokens = DynArray_new(0);
    if (oTokens == NULL)
    {
        fprintf(stderr, "Cannot allocate memory\n");
        exit(EXIT_FAILURE);
    }

    isSuccessful = lexLine(acLine, oTokens);
    if (isSuccessful)
    {
        isSuccessful = syntaticLine(oTokens);
        DynArray_map(oTokens, printAnyTokenWithTokenType, NULL);
    }
    if (isSuccessful)
    {
        printf("%s\n", "valid");
        DynArray_map(oTokens, printAnyTokenWithCommandType, NULL);
    }
    else
    {
        printf("%s\n", "invalid");
    }
    return oTokens;
}

int main(void)
{
    char acLine[MAX_LINE_SIZE];

    while (fgets(acLine, MAX_LINE_SIZE, stdin) != NULL)
    {
        DynArray_T oTokens;
        printf("%%");
        oTokens = executionInit(oTokens, acLine);
        DynArray_map(oTokens, freeToken, NULL);
        DynArray_free(oTokens);
    }
}