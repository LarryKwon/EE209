#define _DEFAULT_SOURCE
#include "dynarray.h"
#include "token.h"
#include "lexical.h"
#include "syntatic.h"

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
    int lexical;
    int syntatic;
    int valid;

    oTokens = DynArray_new(0);
    if (oTokens == NULL)
    {
        fprintf(stderr, "Cannot allocate memory\n");
        exit(EXIT_FAILURE);
    }

    lexical = lexLine(acLine, oTokens);
    if (lexical)
    {
        DynArray_map(oTokens, printAnyTokenWithTokenType, NULL);
        printf("%s\n", "-----------------------");
        syntatic = syntaticLine(oTokens);
    }
    else
    {
        return NULL;
    }
    if (syntatic)
    {
        //printf("%s\n", "valid");
        DynArray_map(oTokens, printAnyTokenWithCommandType, NULL);
    }
    else
    {
        // printf("%s\n", "invalid");
        return NULL;
    }
    return oTokens;
}

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

enum BuiltInCommand
isBuiltIn(DynArray_T oTokens)
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
}

void constructCommands(DynArray_T Tokens, char **commands)
{
    assert(commands);
    int length = DynArray_getLength(Tokens);

    for (int i = 0; i < length; i++)
    {
        char *value = getTokenValue(DynArray_get(Tokens, i));
        commands[i] = strdup(value);
    }
    commands[length] = NULL;
}

int execute(DynArray_T oTokens, char **argv)
{
    int status;
    int length = DynArray_getLength(oTokens);
    char **commands = malloc(length + 1);
    constructCommands(oTokens, commands);
    fflush(NULL);
    pid_t childId = fork();
    if (childId == -1)
    {
        perror(argv[0]);
        return EXIT_FAILURE;
    }
    if (childId == 0)
    {
        if (commands != NULL)
        {
            execvp(commands[0], commands);
            perror(argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        childId = wait(&status);
        free(commands);
        return TRUE;
    }
}

int main(int argc, char *argv[])
{
    char acLine[MAX_LINE_SIZE];
    printf("%% ");
    while (fgets(acLine, MAX_LINE_SIZE, stdin) != NULL)
    {
        DynArray_T oTokens;

        oTokens = executionInit(oTokens, acLine);

        if (oTokens == NULL)
        {
        }
        else
        {
            enum BuiltInCommand builtInCommand = isBuiltIn(oTokens);

            if (builtInCommand != NOTHING)
            {
                BuiltIn_T builtInFunction = BuiltInContext(builtInCommand);
                int builtInResult = (*builtInFunction)(oTokens);
                if (builtInResult == FALSE)
                {
                }
            }
            else
            {
                execute(oTokens, argv);
            }
        }
        printf("%% ");
        DynArray_map(oTokens, freeToken, NULL);
        DynArray_free(oTokens);
    }
}