/* 원윤재 Assignment 5 myish.c*/
#define _DEFAULT_SOURCE
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include "syn.h"
#include "lex.h"
#include "dynarray.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/wait.h>
#include <fcntl.h>
/*-------------------------------------------------------------------*/
// Exit if mySigquitHandler1 is executed. Note that this is installed
// in mySigquitHandler.
void mySigquitHandler1(int iSignal)
{
    _exit(0);
}
/*-------------------------------------------------------------------*/
// If mySigquitHandler is executed, reinstall handler mySigquitHandler1
// for SIGQUIT. Print message to STDOUT, and after 5 seconds send
// SIGALRM signal. Basically, print the message "Type Ctrl-\ again
// within 5 seconds to exit." to the standard output stream.
// If and only if the user indeed types Ctrl-\ again within 5 seconds
// of wall-clock time, then the parent process should terminate.
void mySigquitHandler(int iSignal)
{
    assert(signal(SIGQUIT, mySigquitHandler1) != SIG_ERR);
    char *messageout =
        "\nType Ctrl-\\ again within 5 seconds to exit.\n% ";
    write(STDOUT_FILENO, messageout, strlen(messageout));
    alarm(5);
}
/*-------------------------------------------------------------------*/
// If mySigalrmHandler is executed, reinstall signal handler for
// SIGQUIT and SIGALRM to mySigquitHandler and mySigalrmHandler.
void mySigalrmHandler(int iSignal)
{
    assert(signal(SIGQUIT, mySigquitHandler) != SIG_ERR);
    assert(signal(SIGALRM, mySigalrmHandler) != SIG_ERR);
}
/*-------------------------------------------------------------------*/
// Check if command is built-in command. If it is, then execute
// appropriate task. Return TRUE for successful execution and return
// FALSE if execution false and print error message(if needed)
// to stderr.
int isnotBIcmd(DynArray_T CTokens, char **argv)
{
    struct CToken *token = (struct CToken *)DynArray_get(CTokens, 0);
    // setenv case :
    if (strcmp(getcmdarrayCToken(token)[0], "setenv") == 0)
    {
        if ((DynArray_getLength(CTokens) != 1) ||
            (getcmdarrayCToken(token)[1] == NULL) ||
            (getcmdarrayCToken(token)[3] != NULL) || (getstdinredCToken(token) != NULL) || (getstdoutredCToken(token) != NULL))
        {
            fprintf(stderr, "%s: setenv takes one or two parameters\n", argv[0]);
            return FALSE;
        }
        if (getcmdarrayCToken(token)[2] != NULL)
        {
            if (setenv(getcmdarrayCToken(token)[1],
                       getcmdarrayCToken(token)[2], 1) == -1)
            {
                fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
                return FALSE;
            }
        }
        else
        {
            if (setenv(getcmdarrayCToken(token)[1], "", 1) == -1)
            {
                fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
                return FALSE;
            }
        }
    }
    //unsetenv case :
    else if (strcmp(getcmdarrayCToken(token)[0], "unsetenv") == 0)
    {
        if ((DynArray_getLength(CTokens) != 1) ||
            (getcmdarrayCToken(token)[1] == NULL) ||
            (getcmdarrayCToken(token)[2] != NULL) ||
            (getstdinredCToken(token) != NULL) || (getstdoutredCToken(token) != NULL))
        {
            fprintf(stderr, "%s: unsetenv takes one parameter\n", argv[0]);
            return FALSE;
        }
        if (unsetenv(getcmdarrayCToken(token)[1]) == -1)
        {
            fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
            return FALSE;
        }
    }
    //cd case :
    else if (strcmp(getcmdarrayCToken(token)[0], "cd") == 0)
    {
        if ((DynArray_getLength(CTokens) != 1) ||
            (getcmdarrayCToken(token)[2] != NULL) ||
            (getstdinredCToken(token) != NULL) ||
            (getstdoutredCToken(token) != NULL))
        {
            fprintf(stderr, "%s: cd takes one parameter\n", argv[0]);
            return FALSE;
        }
        if (getcmdarrayCToken(token)[1] != NULL)
        {
            if (chdir(getcmdarrayCToken(token)[1]) == -1)
            {
                fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
                return FALSE;
            }
        }
        else
        {
            if (getenv("HOME") == NULL)
            {
                fprintf(stderr, "%s: cd: HOME variable not set\n", argv[0]);
                return FALSE;
            }
            else if (chdir(getenv("HOME")) == -1)
            {
                fprintf(stderr, "%s: %s\n", argv[0], strerror(errno));
                return FALSE;
            }
        }
    }
    // exit case :
    else if (strcmp(getcmdarrayCToken(token)[0], "exit") == 0)
    {
        if ((DynArray_getLength(CTokens) != 1) ||
            (getcmdarrayCToken(token)[1] != NULL) ||
            (getstdinredCToken(token) != NULL) ||
            (getstdoutredCToken(token) != NULL))
        {
            fprintf(stderr, "%s: exit does not take any parameters\n", argv[0]);
            return FALSE;
        }
        exit(0); // exit with status 0
    }
    else
        return TRUE;
    return FALSE;
}
/*-------------------------------------------------------------------*/
// Perform lexical analysis, syntactic analysis, and check if it is
// built-in command, or consider the command name to be the name of
// file that contains the code to be executed.
// If any of the above step fails, then next step is not executed.
// For non built-in command,
// It forks a child process and pass the file name, along with its
// arguments, to the execvp system call. If the attempt to execute the
// file fails, then print an error message to stderr
// indicating the reason for the failure.
void execute(char *acLine, char **argv)
{
    DynArray_T oTokens;
    DynArray_T CTokens;
    // Create oTokens and CTokens
    oTokens = DynArray_new(0);
    if (oTokens == NULL)
    {
        fprintf(stderr, "Cannot allocate memory\n");
        exit(EXIT_FAILURE);
    }
    CTokens = DynArray_new(0);
    if (CTokens == NULL)
    {
        fprintf(stderr, "Cannot allocate memory\n");
        exit(EXIT_FAILURE);
    }

    if (lexLine(acLine, oTokens, argv))
    {
        if (syn(oTokens, CTokens, argv))
        {
            if (isnotBIcmd(CTokens, argv))
            {

                pid_t *pidarray =
                    (pid_t *)calloc(DynArray_getLength(CTokens), sizeof(pid_t));
                int *fds = NULL;
                // Create adequate amount of pipes for piped commands
                if (DynArray_getLength(CTokens) > 1)
                {
                    fds = (int *)calloc(2 * (DynArray_getLength(CTokens) - 1) + 1, sizeof(int));
                }
                int *temp = fds;
                for (int i = 0; i < 2 * (DynArray_getLength(CTokens) - 1) - 1; i += 2)
                {
                    if (pipe(temp = fds + i) == -1)
                    {
                        fprintf(stderr, "pipe creation failure\n");
                        exit(-1);
                    }
                    int k; //swap just created fds array elements
                    k = *temp;
                    *temp = *(temp + 1);
                    *(temp + 1) = k;
                }

                temp = fds;
                // Fork a child, adjust stdin and stdout, and call execvp
                for (int i = 0; i < DynArray_getLength(CTokens); i++, temp += 2)
                {
                    struct CToken *token = (struct CToken *)DynArray_get(CTokens, i);
                    fflush(NULL);
                    pidarray[i] = fork();

                    if (pidarray[i] < 0)
                    {
                        perror("fork");
                        exit(-1);
                    }
                    else if (pidarray[i] == 0)
                    {
                        //CHILD PROCESS
                        int iFD;
                        int iRet;
                        // redirect stdin as needed
                        if (getstdinredCToken(token))
                        {
                            iFD = open(getstdinredCToken(token), O_RDONLY);
                            if (iFD == -1)
                            {
                                perror(argv[0]);
                                _exit(EXIT_FAILURE);
                            }
                            iRet = dup2(iFD, 0);
                            if (iRet == -1)
                            {
                                perror(getcmdarrayCToken(token)[0]);
                                _exit(EXIT_FAILURE);
                            }

                            iRet = close(iFD);
                            if (iRet == -1)
                            {
                                perror(getcmdarrayCToken(token)[0]);
                                _exit(EXIT_FAILURE);
                            }
                        }
                        // redirect stdout as needed
                        if (getstdoutredCToken(token))
                        {
                            iFD = creat(getstdoutredCToken(token), 0600);
                            if (iFD == -1)
                            {
                                perror(argv[0]);
                                _exit(EXIT_FAILURE);
                            }
                            iRet = dup2(iFD, 1);
                            if (iRet == -1)
                            {
                                perror(getcmdarrayCToken(token)[0]);
                                _exit(EXIT_FAILURE);
                            }

                            iRet = close(iFD);
                            if (iRet == -1)
                            {
                                perror(getcmdarrayCToken(token)[0]);
                                _exit(EXIT_FAILURE);
                            }
                        }
                        // Piped Commands' stdin and stdout redirection
                        if (DynArray_getLength(CTokens) != 1)
                        {
                            //FIRST COMMAND CASE :
                            // redirect stdout and close rest file descriptors
                            if (i == 0)
                            {
                                dup2(*temp, 1);
                                // close all unusing file descriptors
                                temp = fds;
                                if (fds)
                                {
                                    while (*temp)
                                    {
                                        close(*temp++);
                                    }
                                }
                            }
                            //LAST COMMAND CASE :
                            //redirect stdin and close rest file descriptors
                            else if (i == DynArray_getLength(CTokens) - 1)
                            {
                                temp -= 1;
                                dup2(*temp, 0);
                                //close all unusing file descriptors
                                temp = fds;
                                if (fds)
                                {
                                    while (*temp)
                                    {
                                        close(*temp++);
                                    }
                                }
                            }
                            //MIDDLE COMMAND CASE :
                            // redirect stdin and stdout and close rest file descriptors
                            else
                            {
                                dup2(*temp, 1);
                                temp -= 1;
                                dup2(*temp, 0);
                                temp += 1;
                                //close all unusing file descriptors
                                temp = fds;
                                if (fds)
                                {
                                    while (*temp)
                                    {
                                        close(*temp++);
                                    }
                                }
                            }
                        }
                        // Child process' signal handler is reset
                        // for SIGINT and SIGQUIT as default
                        if (signal(SIGINT, SIG_DFL) == SIG_ERR)
                        {
                            perror(argv[0]);
                            exit(EXIT_FAILURE);
                        }
                        if (signal(SIGQUIT, SIG_DFL) == SIG_ERR)
                        {
                            perror(argv[0]);
                            exit(EXIT_FAILURE);
                        }
                        execvp(getcmdarrayCToken(token)[0], getcmdarrayCToken(token));
                        // execvp failure :
                        perror(getcmdarrayCToken(token)[0]);
                        _exit(1);
                    }
                }
                //Parent Process
                //Close all unusing file descriptors
                temp = fds;
                if (fds)
                    while (*temp)
                    {
                        close(*temp++);
                    }

                int status;
                int s = DynArray_getLength(CTokens);

                while (s > 0)
                {
                    wait(&status);
                    pidarray[DynArray_getLength(CTokens) - s] = 0;
                    s--;
                }
                free(fds);
                free(pidarray);
                DynArray_map(CTokens, freeCToken, NULL);
                DynArray_free(CTokens);
            }
        }
        DynArray_map(oTokens, freeToken, NULL);
        DynArray_free(oTokens);
    }
}

/*-------------------------------------------------------------------*/
// At first, (if it is able to,) interpret commands from .ishrc file in
// HOME directory. Then, read commands from stdin stream and execute it
// Repeat until EOF.
int main(int argc, char *argv[])
{
    sigset_t sSet;
    // Make sure SIGINT SIGQUIT SIGALRM aren't blocked
    sigemptyset(&sSet);
    sigaddset(&sSet, SIGINT);
    sigaddset(&sSet, SIGQUIT);
    sigaddset(&sSet, SIGALRM);
    assert(sigprocmask(SIG_UNBLOCK, &sSet, NULL) == 0);
    // Ignore SIGINT, Install mySigquitHandler for SIGQUIT
    // and mySigalrmHandler for SIGALRM
    if (signal(SIGINT, SIG_IGN) == SIG_ERR)
    {
        perror(argv[0]);
        exit(EXIT_FAILURE);
    }
    if (signal(SIGQUIT, mySigquitHandler) == SIG_ERR)
    {
        perror(argv[0]);
        exit(EXIT_FAILURE);
    }
    if (signal(SIGALRM, mySigalrmHandler) == SIG_ERR)
    {
        perror(argv[0]);
        exit(EXIT_FAILURE);
    }

    char acLine[MAX_LINE_SIZE];
    //Read and interpret lines from the
    //file .ishrc in the user's HOME directory, provided that the file
    //exists and is readable. Print each line that it reads.
    FILE *ishrc = NULL;
    char *homedirectory; //path name of .ishrc
    homedirectory = (char *)calloc(1,
                                   strlen(getenv("HOME")) + strlen("/.ishrc") + 1);
    strcat(strcpy(homedirectory, getenv("HOME")), "/.ishrc");
    ishrc = fopen(homedirectory, "r");
    if (ishrc)
    {
        while (fgets(acLine, MAX_LINE_SIZE, ishrc) != NULL)
        {
            printf("%% %s", acLine);
            execute(acLine, argv);
        }
        fclose(ishrc);
    }
    free(homedirectory);
    fflush(NULL);
    // Read command from stdin stream
    printf("%% ");
    while (fgets(acLine, MAX_LINE_SIZE, stdin) != NULL)
    {
        execute(acLine, argv);
        printf("%% ");
    }
    fflush(NULL);
    return 0;
}
