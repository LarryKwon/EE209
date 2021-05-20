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

enum commandState
{
    STATE_START,
    STATE_IN_COMMAND,
    STATE_IN_ARGUMENT,
    STATE_IN_STDIN,
    STATE_IN_STDOUT,
    STATE_IN_PIPE,
    STATE_IN_FILE
};

enum ioState
{
    NONE,
    INPUT_STREAM,
    OUTPUT_STREAM,
    PIPE_STREAM
};

struct ioResult
{
    enum ioState ioState;
    int error;
    const char *errormsg;
};

struct commandResult
{
    enum commandState cState;
    int error;
    const char *errormsg;
};

struct commandResult *commandContext(DynArray_T oTokens, int iIndex, int arrayLength, enum commandState cState)
{
    struct Token *token = DynArray_get(oTokens, iIndex);
    struct commandResult *commandresult = (struct commandResult *)malloc(sizeof(struct commandResult));
    enum TokenType tokenType = token->eType;
    switch (cState)
    {
    case STATE_START:
        if (tokenType == TOKEN_WORD)
        {
            commandresult->error = 0;
            commandresult->errormsg = NULL;
            commandresult->cState = STATE_IN_COMMAND;
            token->cType = COMMAND;
        }

        else
        {
            commandresult->error = 1;
            commandresult->errormsg = "invalid: missing command name\n";
        }
        break;

    case STATE_IN_COMMAND:
        commandresult->error = 0;
        commandresult->errormsg = NULL;
        if (tokenType == TOKEN_WORD)
        {
            commandresult->cState = STATE_IN_ARGUMENT;
            token->cType = ARGUMENT;
        }
        else if (tokenType == TOKEN_STDIN)
        {
            commandresult->cState = STATE_IN_STDIN;
            token->cType = STDIN;
        }
        else if (tokenType == TOKEN_STDOUT)
        {
            commandresult->cState = STATE_IN_STDOUT;
            token->cType = STDOUT;
        }
        else if (tokenType == TOKEN_PIPE)
        {
            commandresult->cState = STATE_IN_PIPE;
            token->cType = PIPE;
        }
        else
        {
            fprintf(stderr, "should not reach here\n");
        }
        break;

    case STATE_IN_ARGUMENT:
        commandresult->error = 0;
        commandresult->errormsg = NULL;
        if (tokenType == TOKEN_WORD)
        {
            commandresult->cState = STATE_IN_ARGUMENT;
            token->cType = ARGUMENT;
        }
        else if (tokenType == TOKEN_STDIN)
        {
            commandresult->cState = STATE_IN_STDIN;
            token->cType = STDIN;
        }
        else if (tokenType == TOKEN_STDOUT)
        {
            commandresult->cState = STATE_IN_STDOUT;
            token->cType = STDOUT;
        }
        else if (tokenType == TOKEN_PIPE)
        {
            commandresult->cState = STATE_IN_PIPE;
            token->cType = PIPE;
        }
        else
        {
            fprintf(stderr, "should not reach here\n");
        }
        break;

    case STATE_IN_STDIN:
        if (iIndex == (arrayLength - 1))
        {
            commandresult->cState = cState;
            commandresult->error = 1;
            commandresult->errormsg = "Invalid: Standard input redirection without input file name\n";
            token->cType = STDIN;
        }
        else
        {
            if (tokenType == TOKEN_WORD)
            {
                commandresult->cState = STATE_IN_FILE;
                commandresult->error = 0;
                commandresult->errormsg = NULL;
                token->cType = FILENAME;
            }
            else
            {
                commandresult->cState = cState;
                commandresult->error = 1;
                commandresult->errormsg = "Invalid: Standard input redirection without input file name\n";
            }
        }
        break;

    case STATE_IN_STDOUT:
        if (iIndex == (arrayLength - 1))
        {
            commandresult->cState = cState;
            commandresult->error = 1;
            commandresult->errormsg = "Invalid: Standard input redirection without output file name\n";
        }
        else
        {
            if (tokenType == TOKEN_WORD)
            {
                commandresult->cState = STATE_IN_FILE;
                commandresult->error = 0;
                commandresult->errormsg = NULL;
                token->cType = FILENAME;
            }
            else
            {
                commandresult->cState = cState;
                commandresult->error = 1;
                commandresult->errormsg = "Invalid: Standard input redirection without output file name\n";
            }
        }
        break;

    case STATE_IN_PIPE:
        if (iIndex == (arrayLength - 1))
        {
            commandresult->cState = cState;
            commandresult->error = 1;
            commandresult->errormsg = "Invalid: No Command after pipe operation\n";
        }
        else
        {
            if (tokenType == TOKEN_WORD)
            {
                commandresult->cState = STATE_IN_COMMAND;
                commandresult->error = 0;
                commandresult->errormsg = NULL;
                token->cType = COMMAND;
            }
            else
            {
                commandresult->cState = cState;
                commandresult->error = 1;
                commandresult->errormsg = "Invalid: No Command after pipe operation\n";
            }
        }
        break;

    case STATE_IN_FILE:
        commandresult->error = 0;
        commandresult->errormsg = NULL;
        if (tokenType == TOKEN_STDIN)
        {
            commandresult->cState = STATE_IN_STDIN;
            token->cType = STDIN;
        }
        else if (tokenType == TOKEN_STDOUT)
        {
            commandresult->cState = STATE_IN_STDOUT;
            token->cType = STDOUT;
        }
        else if (tokenType == TOKEN_PIPE)
        {
            commandresult->cState = STATE_IN_PIPE;
            token->cType = PIPE;
        }
        else if (tokenType == TOKEN_WORD)
        {
            commandresult->cState = STATE_IN_ARGUMENT;
            token->cType = ARGUMENT;
        }
        else
        {
            fprintf(stderr, "should not reach here\n");
        }
        break;
    }

    return commandresult;
}

struct ioResult *ioContext(DynArray_T oTokens, int iIndex, int arrayLength, enum ioState ioState)
{
    struct Token *token = DynArray_get(oTokens, iIndex);
    struct ioResult *ioResult = (struct ioResult *)malloc(sizeof(struct ioResult));
    enum TokenType tokenType = token->eType;
    switch (ioState)
    {
    case NONE:
        ioResult->error = 0;
        ioResult->errormsg = NULL;
        if (tokenType == TOKEN_STDIN)
        {
            ioResult->ioState = INPUT_STREAM;
        }
        else if (tokenType == TOKEN_STDOUT)
        {
            ioResult->ioState = OUTPUT_STREAM;
        }
        else if (tokenType == TOKEN_PIPE)
        {
            ioResult->ioState = PIPE_STREAM;
        }
        else if (tokenType == TOKEN_WORD)
        {
            ioResult->ioState = NONE;
        }
        else
        {
            fprintf(stderr, "should not reach here\n");
        }
        break;

    case INPUT_STREAM:
        if (tokenType == TOKEN_STDIN)
        {
            ioResult->error = 1;
            ioResult->errormsg = "Invalid: Multiple redirection of standard input";
        }
        else if (tokenType == TOKEN_STDOUT)
        {
            ioResult->error = 0;
            ioResult->errormsg = NULL;
            ioResult->ioState = OUTPUT_STREAM;
        }
        else if (tokenType == TOKEN_PIPE)
        {
            ioResult->error = 0;
            ioResult->errormsg = NULL;
            ioResult->ioState = PIPE_STREAM;
        }
        else if (tokenType == TOKEN_WORD)
        {
            ioResult->error = 0;
            ioResult->errormsg = NULL;
            ioResult->ioState = INPUT_STREAM;
        }
        else
        {
            fprintf(stderr, "should not reach here\n");
        }
        break;

    case OUTPUT_STREAM:
        if (tokenType == TOKEN_STDIN)
        {
            ioResult->error = 0;
            ioResult->errormsg = NULL;
            ioResult->ioState = INPUT_STREAM;
        }
        else if (tokenType == TOKEN_STDOUT)
        {
            ioResult->error = 1;
            ioResult->errormsg = "Invalid: Multiple redirection of standard output\n";
        }
        else if (tokenType == TOKEN_PIPE)
        {
            ioResult->error = 1;
            ioResult->errormsg = "Invalid: Multiple redirection of standard output\n";
        }
        else if (tokenType == TOKEN_WORD)
        {
            ioResult->error = 0;
            ioResult->errormsg = NULL;
            ioResult->ioState = OUTPUT_STREAM;
        }
        else
        {
            fprintf(stderr, "should not reach here\n");
        }
        break;
    case PIPE_STREAM:
        if (tokenType == TOKEN_STDIN)
        {
            ioResult->error = 1;
            ioResult->errormsg = "Invalid: Multiple redirection of standard input\n";
        }
        else if (tokenType == TOKEN_STDOUT)
        {
            ioResult->error = 0;
            ioResult->errormsg = NULL;
            ioResult->ioState = OUTPUT_STREAM;
        }
        else if ((tokenType == TOKEN_PIPE) || (tokenType == TOKEN_WORD))
        {
            ioResult->error = 0;
            ioResult->errormsg = NULL;
            ioResult->ioState = PIPE_STREAM;
        }
        else
        {
            fprintf(stderr, "should not reach here\n");
        }
        break;
    }
    return ioResult;
}

int syntaticLine(DynArray_T oTokens)
{
    int iIndex;
    int tokenArrayLength = DynArray_getLength(oTokens);
    enum commandState cState = STATE_START;
    for (iIndex = 0; iIndex < tokenArrayLength; iIndex++)
    {
        struct commandResult *commandresult = commandContext(oTokens, iIndex, tokenArrayLength, cState);
        cState = commandresult->cState;
        if (commandresult->error)
        {
            fprintf(stderr, commandresult->errormsg);
            return FALSE;
        }
        free(commandresult);
    }
    return TRUE;
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
            isSuccessful = syntaticLine(oTokens);
        }
    }
    DynArray_map(oTokens, printAnyTokenWithCommandType, NULL);
    DynArray_map(oTokens, freeToken, NULL);
    DynArray_free(oTokens);
}