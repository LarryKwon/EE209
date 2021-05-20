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

struct Result
{
    enum commandState cState;
    int error;
    char *errormsg;
};

struct Result *commandContext(DynArray_T oTokens, int iIndex, int arrayLength, enum commandState cState)
{
    struct Token *token = DynArray_get(oTokens, iIndex);
    struct Result *result = (strcut Result *)malloc(sizeof(struct Result));
    switch (cState)
    {
    case STATE_START:
        if (token->eType == TOKEN_WORD)
        {
            result->error = 0;
            result->errormsg = NULL;
            result->cState = STATE_IN_COMMAND;
            token->cType = COMMAND;
        }

        else
        {
            result->error = 1;
            result->errormsg = "invalid: missing command name\n";
        }
        break;

    case STATE_IN_COMMAND:
        enum TokenType tokenType = token->eType;
        result->error = 0;
        result->errormsg = NULL;
        if (tokenType == TOKEN_WORD)
        {
            result->cState = STATE_IN_ARGUMENT;
            token->cType = ARGUMENT;
        }
        else if (tokenType == TOKEN_STDIN)
        {
            result->cState = STATE_IN_STDIN;
            token->cType = STDIN;
        }
        else if (tokenType == TOKEN_STDOUT)
        {
            result->cState = STATE_IN_STDOUT;
            token->cType = STDOUT;
        }
        else if (tokenType == TOKEN_PIPE)
        {
            result->cState = STATE_IN_PIPE;
            token->cType = PIPE;
        }
        else
        {
            fprintf(stderr, "should not reach here");
        }
        break;

    case STATE_IN_ARGUMENT:
        enum TokenType tokenType = token->eType;
        result->error = 0;
        result->errormsg = NULL;
        if (tokenType == TOKEN_WORD)
        {
            result->cState = STATE_IN_ARGUMENT;
            token->cType = ARGUMENT;
        }
        else if (tokenType == TOKEN_STDIN)
        {
            result->cState = STATE_IN_STDIN;
            token->cType = STDIN;
        }
        else if (tokenType == TOKEN_STDOUT)
        {
            result->cState = STATE_IN_STDOUT;
            token->cType = STDOUT;
        }
        else if (tokenType == TOKEN_PIPE)
        {
            result->cState = STATE_IN_PIPE;
            token->cType = PIPE;
        }
        else
        {
            fprintf(stderr, "should not reach here");
        }
        break;

    case STATE_IN_STDIN:
        enum TokenType tokenType = token->eType;
        if (iIndex == (arrayLength - 1))
        {
            result->cState = cState;
            result->error = 1;
            result->errormsg = "Invalid: Standard input redirection without input file name\n";
            token->cType = STDIN;
        }
        else
        {
            if (tokenType == Token_WORD)
            {
                result->cState = STATE_IN_FILE;
                result->error = 0;
                result->errormsg = NULL;
                token->cType = FILENAME;
            }
            else
            {
                result->cState = cState;
                result->error = 1;
                result->errormsg = "Invalid: Standard input redirection without input file name\n"
            }
        }
        break;

    case STATE_IN_STDOUT:
        enum TokenType tokenType = token->eType;
        if (iIndex == (arrayLength - 1))
        {
            result->cState = cState;
            result->error = 1;
            result->errormsg = "Invalid: Standard input redirection without output file name\n";
        }
        else
        {
            if (tokenType == Token_WORD)
            {
                result->cState = STATE_IN_FILE;
                result->error = 0;
                result->errormsg = NULL;
                token->cType = FILENAME;
            }
            else
            {
                result->cState = cState;
                result->error = 1;
                result->errormsg = "Invalid: Standard input redirection without output file name\n"
            }
        }
        break;

    case STATE_IN_PIPE:
        enum TokenType tokenType = token->eType;
        if (iIndex == (arrayLength - 1))
        {
            result->cState = cState;
            result->error = 1;
            result->errormsg = "Invalid: No Command after pipe operation\n";
        }
        else
        {
            if (tokenType == Token_WORD)
            {
                result->cState = STATE_IN_COMMAND;
                result->error = 0;
                result->errormsg = NULL;
                token->cType = COMMAND;
            }
            else
            {
                result->cState = cState;
                result->error = 1;
                result->errormsg = "Invalid: No Command after pipe operation\n"
            }
        }
        break;

    case STATE_IN_FILE:
        enum TokenType tokenType = token->eType;
        result->error = 0;
        result->errormsg = NULL;
        if (tokenType == TOKEN_STDIN)
        {
            result->cState = STATE_IN_STDIN;
            token->cType = STDIN;
        }
        else if (tokenType == TOKEN_STDOUT)
        {
            result->cState = STATE_IN_STDOUT;
            token->cType = STDOUT;
        }
        else if (tokenType == TOKEN_PIPE)
        {
            result->cState = STATE_IN_PIPE;
            token->cType = PIPE;
        }
        else if (tokenType == TOKEN_WORD)
        {
            result->cState = STATE_IN_ARGUMENT;
            token->cType = ARGUMENT;
        }
        else
        {
            fprintf(stderr, "should not reach here");
        }
        break;
    }

    return result;
}

int syntaticLine(DynArray_T oTokens)
{
    int iIndex;
    int tokenArrayLength = DynArray_getLength(oTokens);
    enum commandState cState = STATE_START;
    for (iIndex = 0; iIndex < tokenArrayLength; i++)
    {
        struct Result *result = commandContext(oTokens, iIndex, tokenArrayLength, cState);
        cState = result->cState;
        if (result->error)
        {
            fprintf(stderr, result->errormsg);
            return FALSE;
        }
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