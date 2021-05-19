#include "syntatic.h"
#include "token.h"
#include "dynarray.h"
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

void fillCommandtype(DynArray_T oTokens)
{
    int lineLength = DynArray_getLength(oTokens);
    int commandIndex = 0;
    struct Token *psToken;
    for (int i = 0; i < lineLength; i++)
    {
        psToken = (struct Token *)DynArray_get(oTokens, i);
        switch (commandIndex)
        {
        case 0:
            psToken->cType = COMMAND;
            break;

        default:
            if (pcToken->eType == TOKEN_OUTPUT)
            {
                if (strcmp(psToken->pcValue, "|") == 0)
                {
                    psToken->cType == PIPE;
                }
                else if (strcmp(psToken->pcValue, "<") == 0)
                {
                    psToken->cType == STDIN;
                }
                else if (strcmp(psToken->pcValue, ">") == 0)
                {
                    psToken->cType == STDOUT;
                }
                else
                {
                    fprintf(stderr, "error may be in lexical analysis");
                }
                commandIndex = 0;
            }
            else
            {
                psToken->cType = ARGUMENT;
            }
            break;
        }
    }
}

int validCheck(DynArray_T oTokens)
{

    enum SynState
    {
        STATE_COMMANd,
        STATE_ARGUMENT,
        STATE_FILE,
    };
    int lineLength = DynArray_getLength(oTokens);
    int commandIndex = 0;
    struct Token *psToken;
}

int syntaticLine(DynArray_T oTokens)
{
    fillCommandtype(oTokens);
}

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
            syntaticLine(oTokens);
        }
    }
    DynArray_map(oTokens, freeToken, NULL);
    DynArray_free(oTokens);
}