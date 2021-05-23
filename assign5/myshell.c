#define _DEFAULT_SOURCE
#include "dynarray.h"
#include "token.h"
#include "lexical.h"
#include "syntatic.h"
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
        //DynArray_map(oTokens, printAnyTokenWithTokenType, NULL);
        //printf("%s\n", "-----------------------");
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

void commandsConstructor(DynArray_T Tokens, char **commands)
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
    commandsConstructor(oTokens, commands);
    
    //commands를 모아놓은 배열을 제작해야한다.

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