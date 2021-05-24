#include "dynarray.h"
#include "token.h"
#include "builtin.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

enum
{
    FALSE,
    TRUE

};

enum
{
    MAX_LINE_SIZE = 1024
};

int execCd(DynArray_T oTokens)
{
    int commandLine = DynArray_getLength(oTokens);

    if (commandLine > 2)
    {
        fprintf(stderr, "cd Takes one parameter\n");

        return FALSE;
    }
    else if (commandLine == 1)
    {
        char *home = getenv("HOME");
        if (home == NULL)
        {
            fprintf(stderr, "%s\n", strerror(errno));

            return FALSE;
        }
        else
        {
            if (chdir(home) == -1)
            {
                fprintf(stderr, "%s\n", strerror(errno));
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
    if (commandLine > 3 || commandLine == 1)
    {
        fprintf(stderr, "setenv Takes one or two parameter\n");
        return FALSE;
    }
    else
    {
        enum CommandType cType = getCommandType(DynArray_get(oTokens, 1));
        if (cType == STDIN || cType == STDOUT || cType == PIPE)
        {
            fprintf(stderr, "setenv takes one or two parameter\n");
            return FALSE;
        }
        else
        {
            char *firstParam = getTokenValue(DynArray_get(oTokens, 1));
            int result;
            if (commandLine == 3)
            {
                char *secondParam = getTokenValue(DynArray_get(oTokens, 2));
                result = setenv(firstParam, secondParam, 1);
            }
            else
            {
                result = setenv(firstParam, "", 1);
            }
            if (result == -1)
            {
                fprintf(stderr, "%s\n", strerror(errno));
                return FALSE;
            }
        }
    }
    return TRUE;
}

int execUnsetenv(DynArray_T oTokens)
{
    int commandLine = DynArray_getLength(oTokens);
    if (commandLine != 2)
    {
        fprintf(stderr, "unsetenv Takes one parameter\n");
        return FALSE;
    }
    else
    {
        char *firstParam = getTokenValue(DynArray_get(oTokens, 1));
        int result = unsetenv(firstParam);
        if (result == -1)
        {
            fprintf(stderr, "%s\n", strerror(errno));
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
        fprintf(stderr, "exit does not take any parameters\n");
        return FALSE;
    }
    else
    {
        exit(0);
    }
    return TRUE;
}

enum BuiltInCommand isBuiltIn(DynArray_T oTokens)
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
        fprintf(stderr, "should not reach here\n");

        break;
    }
    return NULL;
}
