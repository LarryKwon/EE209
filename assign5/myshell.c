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
        //DynArray_map(oTokens, printAnyTokenWithCommandType, NULL);
    }
    else
    {
        // printf("%s\n", "invalid");
        return NULL;
    }
    return oTokens;
}

static int findCommandType(const void *Token, const void *command)
{
    Token_T token = (Token_T)Token;
    enum CommandType *commandType = (enum CommandType *)command;
    if (getCommandType(token) == *commandType)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

DynArray_T commandsSpliter(DynArray_T oTokens, DynArray_T cTokens)
{
    assert(oTokens);
    assert(cTokens);
    int length = DynArray_getLength(oTokens);

    int index = 0;
    int prevIndex = 0;
    enum CommandType *commandTypes;
    *commandTypes = PIPE;
    while (index < length)
    {
        index = DynArray_search(oTokens, commandTypes, findCommandType);

        if (index == -1)
        {
            index = length;
        }

        //하나의 command Line을 담는 DynArray
        DynArray_T command = DynArray_new(0);
        for (int i = prevIndex; i < index; i++)
        {
            DynArray_add(command, DynArray_removeAt(oTokens, 0));
        }
        DynArray_map(command, printAnyTokenWithCommandType, NULL);
        DynArray_add(cTokens, command);
        prevIndex = index + 1;
        if (DynArray_getLength(oTokens) == 0)
        {
            continue;
        }
        else
        {
            DynArray_removeAt(oTokens, 0);
        }
    }
    DynArray_free(oTokens);
    return cTokens;
}

char **commandsConstructor(DynArray_T Tokens, char **command)
{
    int length = DynArray_getLength(Tokens);
    command = calloc(length + 1, sizeof(char *));
    for (int i = 0; i < length; i++)
    {
        char *value = getTokenValue(DynArray_get(Tokens, i));
        command[i] = strdup(value);
    }
    command[length] = NULL;

    return command;
}

static int **pipeConstructor(int pipeNumbers)
{
    int **pipes = (int **)calloc(pipeNumbers, sizeof(int *));

    for (int i = 0; i < pipeNumbers; i++)
    {
        *pipes = calloc(2, sizeof(int));
    }
    return pipes;
}

void recursivePipe(int commandIndex, char ***commandLines, char **command, int commandSize, int pipeNumbers, int **pipes)
{
    if (commandIndex < commandSize)
    {
        if (pipe(pipes[commandIndex]) == -1)
        {
            exit(-1);
        }
    }
    fflush(NULL);
    pid_t childId = fork();
    switch (childId)
    {
    case -1:
        perror(argv[0]);
        return EXIT_FAILURE;
        break;
    case 0:
        if (command != NULL)
        {
            //1번 pipe설정: pipe가 있다면, 첫번째꺼일꺼니까, stdout을 pipes[0][]
            if (pipeNumbers > 0)
            {
                close(fds[0]);
                dup2(fds[1], 1); /* stdout */
                close(fds[1]);
            }
            execvp(command[0], command);
            perror(argv[0]);
            exit(EXIT_FAILURE);
        }
    default:
        commandIndex += 1;
        recursivePipe(commandIndex, commandlines, commandLines[commandIndex], commandSize, pipeNumbers, pipes);
        wait(&status);

        //commandLine에 있는 strdup한거 다 free시켜야함
        free(commandLines[i]);
    }
}

int execute(DynArray_T oTokens, char **argv)
{
    int status;
    int length = DynArray_getLength(oTokens);

    //DynArray_T를 element로 가지는 DynArray
    DynArray_T cTokens = DynArray_new(0);
    cTokens = commandsSpliter(oTokens, cTokens);
    int commandSize = DynArray_getLength(cTokens);

    //commandLines: 각 Command를 가지고 있는 배열
    char ***commandLines = (char ***)calloc(commandSize, sizeof(char **));
    for (int i = 0; i < commandSize; i++)
    {
        commandLines[i] = commandsConstructor(DynArray_get(cTokens, i), commandLines[i]);
    }
    // //cTokens을 Array화 시킨 commandLines
    // void **commandLines = (void **)calloc(commandSize, sizeof(struct DynArray *));
    // DynArray_toArray(cTokens, commandLines);

    // for (int i = 0; i < commandSize; i++)
    // {
    //     DynArray_map(commandLines[i], printAnyTokenWithCommandType, NULL);
    // }

    //pipe creation
    int pipeNumbers = commandSize - 1;
    int **pipes = pipeConstructor(pipeNumbers);

    // for (int i = 0; i < pipeNumbers; i++)
    // {
    //     if (pipe(pipes[i]) == -1)
    //     {
    //         exit(-1);
    //     }
    // }

    // pid_t *processIds = calloc(commandSize, sizeof(pid_t *));

    if (pipeNumbers > 0)
    {
        if (pipe(pipes[0]) == -1)
        {
            exit(-1);
        }
    }
    fflush(NULL);
    pid_t childId = fork();
    int commandIndex = 0;
    switch (childId)
    {
    case -1:
        perror(argv[0]);
        return EXIT_FAILURE;
        break;
    case 0:
        if (commandLines[0] != NULL)
        {
            //1번 pipe설정: pipe가 있다면, 첫번째꺼일꺼니까, stdout을 pipes[0][]
            if (pipeNumbers > 0)
            {
                close(fds[0]);
                dup2(fds[1], 1); /* stdout */
                close(fds[1]);
            }
            execvp(commandLines[0][0], commandLines[0]);
            perror(argv[0]);
            exit(EXIT_FAILURE);
        }
    default:
        wait(&status);
        if (commandSize > 1)
        {
            commandIndex += 1;
            recursivePipe(commandIndex, commandLines, commandLines[commandIndex], commandSize, pipeNumbers, pipes);
        }
        //commandLine에 있는 strdup한거 다 free시켜야함
        free(commandLines[i]);
    }
    wait(&status);
    return TRUE;

    // fflush(NULL);
    // pid_t childId = fork();
    // if (childId == -1)
    // {
    //     perror(argv[0]);
    //     return EXIT_FAILURE;
    // }
    // if (childId == 0)
    // {
    //     if (commands != NULL)
    //     {
    //         execvp(commands[0], commands);
    //         perror(argv[0]);
    //         exit(EXIT_FAILURE);
    //     }
    // }
    // else
    // {
    //     childId = wait(&status);
    //     free(commands);
    //     return TRUE;
    // }
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

        //DynArray_map(oTokens, freeToken, NULL);
        //DynArray_free(oTokens);
    }
}