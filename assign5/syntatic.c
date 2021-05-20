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
        result->error = 0;
        result->errormsg = NULL;
        if (token->eType == TOKEN_WORD)
        {
            result->cState = STATE_IN_ARGUMENT;
            token->cType = ARGUMENT;
        }
        else if (token->eType == TOKEN_STDIN)
        {
            result->cState = STATE_IN_STDIN;
            token->cType = STDIN;
        }
        else if (token->eType == TOKEN_STDOUT)
        {
            result->cState = STATE_IN_STDOUT;
            token->cType = STDOUT;
        }
        else if (token->eType == TOKEN_PIPE)
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
        result->error = 0;
        result->errormsg = NULL;
        if (token->eType == TOKEN_WORD)
        {
            result->cState = STATE_IN_ARGUMENT;
            token->cType = ARGUMENT;
        }
        else if (token->eType == TOKEN_STDIN)
        {
            result->cState = STATE_IN_STDIN;
            token->cType = STDIN;
        }
        else if (token->eType == TOKEN_STDOUT)
        {
            result->cState = STATE_IN_STDOUT;
            token->cType = STDOUT;
        }
        else if (token->eType == TOKEN_PIPE)
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

        break;
    case STATE_IN_STDOUT:
        break;
    case STATE_IN_PIPE:
        break;
    case STATE_IN_FILE:
        break;
    }

    return result;
}

int syntaticLine(DynArray_T oTokens)
{
    fillCommandtype(oTokens);
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
            syntaticLine(oTokens);
        }
    }
    DynArray_map(oTokens, freeToken, NULL);
    DynArray_free(oTokens);
}