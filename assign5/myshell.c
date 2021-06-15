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
        exit(1);
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
    while (TRUE)
    {
        index = DynArray_search(oTokens, commandTypes, findCommandType);

        if (index == -1)
        {
            index = DynArray_getLength(oTokens);
        }

        //하나의 command Line을 담는 DynArray
        DynArray_T command = DynArray_new(0);
        for (int i = 0; i < index; i++)
        {
            DynArray_add(command, DynArray_removeAt(oTokens, 0));
        }
        DynArray_map(command, printAnyTokenWithCommandType, NULL);
        DynArray_add(cTokens, command);
        // prevIndex = index + 1;
        if (DynArray_getLength(oTokens) == 0)
        {
            break;
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
        pipes[i] = calloc(2, sizeof(int));
    }
    return pipes;
}

int execute(DynArray_T oTokens, char **argv)
{
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

    //pipe creation
    // 이 때 모든 process가 모든 파이프의 파일 descriptor를 가지고 있어야한다.
    int pipeNumbers = commandSize - 1;
    int **pipes = pipeConstructor(pipeNumbers);

    if (pipeNumbers > 0)
    {
        for (int i = 0; i < pipeNumbers; i++)
        {
            if (pipe(pipes[i]) == -1)
            {
                exit(-1);
            }
        }
        //우선 첫번째 파이프는 이렇게 조정
        printf("%s %d\n", "pipe1", 0);
        close(pipes[0][0]);
        printf("%s %d\n", "pipe1", 1);
        dup2(pipes[0][1], 1); /* stdout */
        printf("%s %d\n", "pipe1", 2);
        close(pipes[0][1]);
        printf("%s %d\n", "pipe1", 3);
    }
    //병렬적으로 프로세스 진행
    // 최상위 shell -> 여러 개의 자식 프로세스를 실행
    int commandIndex = 0;
    while (commandIndex < commandSize)
    {
        fflush(NULL);
        pid_t childId = fork();
        if (childId == -1)
        {
            perror(argv[0]);
            exit(1);
        }
        // 자식 프로세스
        else if (childId == 0)
        {
            //명령어 실행
            /*첫번째 명령어일 때
              명령어 그냥 실행  
            */
            if (commandIndex == 0)
            {
            }
            /* 마지막일 때
                pipe를 stdin에만 붙이고
                명령어 실행
            */
            else if (commandIndex == commandSize - 1)
            {
                /* stdin */
                close(pipes[commandIndex - 1][1]);
                dup2(pipes[commandIndex - 1][0], 0);
                dup2(STDOUT_FILENO, 1);
                close(pipes[commandIndex][0]);
            }
            /* 중간일 때
                commandIndex에 맞는 pipe를 stdin과 stdout에 붙인 후
                명령어 실행
            */
            else
            {
                /* stdin */
                close(pipes[commandIndex - 1][1]);
                dup2(pipes[commandIndex - 1][0], 0);
                /* stdout */
                close(pipes[commandIndex][0]);
                dup2(pipes[commandIndex][1], 1);
            }
            printf("%s %d\n", "child", commandIndex);
            execvp(commandLines[commandIndex][0], commandLines[commandIndex]);
            perror(argv[0]);
            exit(1);
        }
        // commandIndex = 0 이라면, stdout을 원래 STDOUT_FILENO으로 돌려놓기
        if (commandIndex == 0)
        {
            dup2(STDOUT_FILENO, 1);
        }
        commandIndex += 1;
    }
    //공통적으로 사용하지 않는 파일 디스크립터 전부 닫기
    for (int i = 0; i < pipeNumbers; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    //모든 자식들 끝날 때까지 기다리기
    int status;
    while ((wait(&status)) > 0)
        ;

    //commandLine에 있는 strdup한거 다 free시켜야함
    free(commandLines[0]);
    return TRUE;
}

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