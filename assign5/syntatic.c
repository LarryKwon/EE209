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

enum
{
    FALSE,
    TRUE
};

int main(void)
{

    char acLine[MAX_LINE_SIZE];
    int isSuccessful;
    int valid;

    DynArray_T oTokens;

    DynArray_T commandTokens = DynArray_new(0);

    while (fgets(acLine, MAX_LINE_SIZE, stdin) != NULL)
    {
        oTokens = DynArray_new(0);
        if (oTokens == NULL)
        {
            fprintf(stderr, "Cannot allocate memory\n");
            exit(EXIT_FAILURE);
        }

        isSuccessful = lexLine(acLine, oTokens);
        if (isSuccessful)
        {
            DynArray_map(oTokens, printAnyToken, NULL);
        }
    }
    DynArray_map(oTokens, freeToken, NULL);
    DynArray_free(oTokens);
}