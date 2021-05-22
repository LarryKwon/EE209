#include "lexical.h"
#include "syntatic.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
enum
{
    FALSE,
    TRUE
};

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
    int commandLine = DynArray_getLength(oTokens);

    if (commandLine > 2)
    {
        fprintf(stderr, "cd Takes one parameter");
        return FALSE;
    }
    else if (commandLine == 1)
    {
        char *home = getenv("HOME");
        if (home == NULL)
        {
            fprintf(stderr, "%s\n", strerr(errno));
            return FALSE;
        }
        else
        {
            if (chdir(home) == -1)
            {
                fprintf(stderr, "%s\n", strerr(errno));
                return FALSE;
            }
        }
    }
    else
    {
        Token_T arg = DynArray_get(oTokens, 1);
        char *argValue = getTokenValue(arg);
        if (chdir(argValue) == -1)
        {
            fprintf(stderr, "%s\n", strerror(errno));
            return FALSE;
        }
    }
    return TRUE;
}

int execSetenv(DynArray_T oTokens)
{
    int commandLine = DynArray_getLength(oTokens);
    enum Commandtype cType = getCommandType(DynArray_get(oTokens, 1));
    if (commandLine > 3 || commandLine == 1)
    {
        fprintf(stderr, "setenv Takes one or two parameter");
        return FALSE;
    }
    else if (cType == STDIN || cType == STDOUT || cType == PIPE)
    {
        fprintf(stderr, "setenv takes one or two parameter");
        return FALSE;
    }
    else
    {
        char *firstParam = getTokenValue(DynArray_get(oTokens, 1));
        char *secondParam = getTokenValue(DynArray_get(oTokens, 2));
        int result = setenv(firstParam, secondParam, 1);
        if (result == -1)
        {
            fprintf(stderr, "%s\n", strerror(errno));
            return FALSE;
        }
    }
    return TRUE;
}

int execUnsetenv(DynArray_T oTokens)
{
    int commandLine = DynArray_getLength(oTokens);
    if (commandLine != 2)
    {
        fprintf(stderr, "unsetenv Takes one parameter");
        return FALSE;
    }
    else
    {
        char *firstParam = getTokenValue(DynArray_get(oTokens, 1));
        int result = unsetenv(firstParam);
        if (result == -1)
        {
            fprint(stderr, "%s\n", strerror(errno));
            return FALSE;
        }
    }
    return TRUE;
}

int execExit(DynArray_T oTokens)
{
    int commandLine = DynArray_getLength(oTokens);
    if (commandLine != 1)
    {
        fprintf(stderr, "exit does not take any parameters");
        return FALSE;
    }
    else
    {
        exit(0);
    }
    return TRUE;
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