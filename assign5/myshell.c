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

enum BuiltInCommand
{
    CD,
    SETENV,
    UNSETENV,
    EXIT,
    NOTHING
};

typedef int (*BuiltIn_T)(DynArray_T);

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

int execCd(DynArray_T oTokens)
{
}

int execSetenv(DynArray_T oTokens)
{
}

int execUnsetenv(DynArray_T oTokens)
{
}

int execExit(DynArray_T oTokens)
{
}

enum BuiltInCommand
isBuiltIn(DynArray_T otokens)
{
    Token_T token = DynArray_get(oTokens, 0);

    char *pcValue = getTokenValue(token);
    if (pcValue == NULL)
    {
        fprintf(stderr, "Cannot allocate memory\n");
    }
    if (strcmp(pcValue, "cd") == 0)
    {
        free(pcValue);
        return CD;
    }
    else if (strcmp(pcValue, "setenv") == 0)
    {
        free(pcValue);
        return SETENV;
    }
    else if (strcmp(pcValue, "unsetenv") == 0)
    {
        free(pcValue);
        return UNSETENV;
    }
    else if (strcmp(pcValue, "exit") == 0)
    {
        free(pcValue);
        return EXIT;
    }
    else
    {
        free(pcValue);
        return NOTHING;
    }
}

BuiltIn_T BuiltInContext(enum BuiltInCommand builtInCommand)
{

    assert(builtInCommand != NOTHING);

    switch (builtInCommand)
    {
    case CD:
        return &execCd;
        break;

    case SETENV:
        return &execSetenv;
        break;

    case UNSETENV:
        return &execUnsetenv;
        break;

    case EXIT:
        return &execExit;
        break;

    case NOTHING:
        fprintf(stderr, "should not reach here\n") break;
    }
}

int main(int argc, char *argv[])
{
    char acLine[MAX_LINE_SIZE];

    while (fgets(acLine, MAX_LINE_SIZE, stdin) != NULL)
    {
        DynArray_T oTokens;
        printf("%%");
        oTokens = executionInit(oTokens, acLine);

        enum BuiltInCommand builtInCommand = isBuiltIn(oTokens);

        if (builtInCommand != NOTHING)
        {
            BuiltIn_T builtInFunction = BuiltInContext(builtInCommand);
            (*builtInfunction)(oTokens);
        }
        else
        {
            execute();
        }
        DynArray_map(oTokens, freeToken, NULL);
        DynArray_free(oTokens);
    }
}