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
    enum CommandType *commandTypes = calloc(1, sizeof(enum CommandType));
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
    free(commandTypes);
    commandTypes = NULL;
    DynArray_free(oTokens);
    return cTokens;
}

char **commandsConstructor(DynArray_T Tokens, char **command)
{
    int length = DynArray_getLength(Tokens);
    command = calloc(length + 1, sizeof(char *));
    char *redirection = NULL;
    enum CommandType commandType;
    int commandIndex = 0;
    for (int i = 0; i < length; i++)
    {
        commandType = getCommandType(DynArray_get(Tokens, i));
        if (commandType == STDIN)
        {
            // redirection = (char *)calloc(1, sizeof(char *));
            if (redirection == NULL)
            {
                redirection = calloc(13, sizeof(char));
                strcpy(redirection, "STDIN");
                printf("%s\n", redirection);
            }
            else
            {
                strcat(redirection, "STDIN");
                printf("%s\n", redirection);
            }
        }
        else if (commandType == STDOUT)
        {
            // redirection = (char *)calloc(1, sizeof(char *));
            if (redirection == NULL)
            {
                redirection = calloc(13, sizeof(char));
                strcpy(redirection, "STDOUT");
                printf("%s\n", redirection);
            }
            else
            {
                strcat(redirection, "STDOUT");
                printf("%s\n", redirection);
            }
        }
        else if (commandType == COMMAND || commandType == ARGUMENT)
        {
            char *value = getTokenValue(DynArray_get(Tokens, i));
            command[commandIndex] = strdup(value);
            commandIndex += 1;
            free(value);
        }
    }
    if (redirection == NULL)
    {
        command[length] = redirection;
    }
    else
    {
        command[length] = strdup(redirection);
        free(redirection);
    }

    return command;
}
char **redirectionFiles(DynArray_T Tokens, char **files)
{
    int length = DynArray_getLength(Tokens);
    files = (char **)calloc(2, sizeof(char *));
    char *stdinFile = NULL;
    char *stdoutFile = NULL;
    for (int i = 0; i < length; i++)
    {
        if (getCommandType(DynArray_get(Tokens, i)) == STDIN)
        {
            //stdinFile = calloc(1, sizeof(char *));
            stdinFile = getTokenValue(DynArray_get(Tokens, i + 1));
        }
        else if (getCommandType(DynArray_get(Tokens, i)) == STDOUT)
        {
            //stdoutFile = calloc(1, sizeof(char *));
            stdoutFile = getTokenValue(DynArray_get(Tokens, i + 1));
        }
        files[0] = stdinFile;
        files[1] = stdoutFile;
    }
    // printf("%s\n", '\0');
    // printf("%s\n", files[1]);
    return files;
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
    char ***fileNames = (char ***)calloc(commandSize, sizeof(char **));
    int *commandLength = (int *)calloc(commandSize, sizeof(int));
    for (int i = 0; i < commandSize; i++)
    {
        commandLines[i] = commandsConstructor(DynArray_get(cTokens, i), commandLines[i]);
        commandLength[i] = DynArray_getLength(DynArray_get(cTokens, i));
        fileNames[i] = redirectionFiles(DynArray_get(cTokens, i), commandLines[i]);
        //printf("%s\n", fileNames[i][1]);
        //printf("%s\n", fileNames[i][0]);
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
                if (pipeNumbers != 0)
                {
                    dup2(pipes[0][1], 1); /* stdout */
                    for (int i = 0; i < pipeNumbers; i++)
                    {
                        close(pipes[i][1]);
                        close(pipes[i][0]);
                    }
                }
                printf("%s %d\n", "child first case", getpid());
            }
            /* 마지막일 때
                pipe를 stdin에만 붙이고
                명령어 실행
            */
            else if (commandIndex == commandSize - 1)
            {
                /* stdin */
                printf("%s %d\n", "child last case", getpid());
                dup2(pipes[commandIndex - 1][0], 0);
                for (int i = 0; i < pipeNumbers; i++)
                {
                    close(pipes[i][0]);
                    close(pipes[i][1]);
                }
                printf("%s %d\n", "last child", pipes[commandIndex - 1][1]);
            }
            /* 중간일 때
                commandIndex에 맞는 pipe를 stdin과 stdout에 붙인 후
                명령어 실행
            */
            else
            {
                printf("%s %d\n", "child middle case", getpid());
                /* stdin */
                dup2(pipes[commandIndex - 1][0], 0);
                /* stdout */
                dup2(pipes[commandIndex][1], 1);
                for (int i = 0; i < pipeNumbers; i++)
                {
                    close(pipes[i][1]);
                    close(pipes[i][0]);
                }
            }
            printf("%s %d\n", "child execution", getpid());

            int length = commandLength[commandIndex];
            char *isRedirection = commandLines[commandIndex][length];
            // printf("%s\n", "printf error");
            // printf("%s\n", isRedirection);

            if (isRedirection != NULL)
            {
                if (strstr(isRedirection, "STDIN") != NULL)
                {
                    //STDIN File Descriptor 조작
                    printf("stdin filename: %s\n", fileNames[commandIndex][0]);
                    int stdinFd = open(fileNames[commandIndex][0], O_RDONLY);
                    if (stdinFd == -1)
                    {
                        perror(argv[0]);
                        _exit(EXIT_FAILURE);
                    }
                    int stdinRet = dup2(stdinFd, 0); // stdin
                    if (stdinRet == -1)
                    {
                        perror(fileNames[commandIndex][0]);
                        _exit(EXIT_FAILURE);
                    }
                    stdinRet = close(stdinFd);
                    if (stdinRet == -1)
                    {
                        perror(fileNames[commandIndex][0]);
                        _exit(EXIT_FAILURE);
                    }

                    // free(isRedirection);
                    commandLines[commandIndex][length] = NULL;
                }

                if (strstr(isRedirection, "STDOUT") != NULL)
                {
                    //STDOUT File Descriptor 조작
                    printf("stdout filename: %s\n", fileNames[commandIndex][1]);
                    int stdoutFd = creat(fileNames[commandIndex][1], 0600);
                    if (stdoutFd == -1)
                    {
                        perror(argv[0]);
                        _exit(EXIT_FAILURE);
                    }
                    int stdoutRet = dup2(stdoutFd, 1); // stdout
                    if (stdoutRet == -1)
                    {
                        perror(fileNames[commandIndex][0]);
                        _exit(EXIT_FAILURE);
                    }
                    stdoutRet = close(stdoutFd);
                    if (stdoutRet == -1)
                    {
                        perror(fileNames[commandIndex][0]);
                        _exit(EXIT_FAILURE);
                    }
                    // free(isRedirection);
                    commandLines[commandIndex][length] = NULL;
                }

                free(isRedirection);
            }

            execvp(commandLines[commandIndex][0], commandLines[commandIndex]);
            perror(argv[0]);
            exit(1);
        }
        printf("%s %d\n", "commandIndex", commandIndex);
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
    int child;
    int s = commandSize;

    while (s > 0)
    {
        child = wait(&status);
        printf("child process result: %d\n", child);
        printf("child process %d, parent process %d\n", child, getpid());
        s--;
    }
    // do
    // {
    //     waitpid(-1, &status, WNOHANG);
    //     printf("child process result: %d\n", child);
    //     printf("child process %d, parent process %d\n", child, getpid());
    // } while (child >= 0);
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