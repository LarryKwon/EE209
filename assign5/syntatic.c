#include "token.h"
#include "dynarray.h"
#include "lexical.h"
#include "syntatic.h"

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

// struct Token
// {
//    enum TokenType eType;
//    /* The type of the token. */

//    char *pcValue;
//    /* The string which is the token's value. */

//    enum CommandType cType;
// };

static int isLastToken(int iIndex, int arrayLength)
{
    if (iIndex == (arrayLength - 1))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

struct commandResult *commandContext(DynArray_T oTokens, int iIndex, int arrayLength, enum commandState cState)
{
    struct Token *token = (struct Token *)DynArray_get(oTokens, iIndex);
    struct commandResult *commandresult = (struct commandResult *)malloc(sizeof(struct commandResult));
    if (commandresult == NULL)
    {
        return NULL;
    }
    enum TokenType tokenType = getTokenType(token);
    switch (cState)
    {
    case STATE_START:
        if (tokenType == TOKEN_WORD)
        {
            commandresult->error = 0;
            commandresult->errormsg = NULL;
            commandresult->cState = STATE_IN_COMMAND;
            setCommandType(token, COMMAND);
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
            setCommandType(token, ARGUMENT);
        }
        else if (tokenType == TOKEN_STDIN)
        {
            if (isLastToken(iIndex, arrayLength))
            {
                commandresult->error = 1;
                commandresult->errormsg = "Invalid: Standard input redirection without file name\n";
            }
            else
            {
                commandresult->cState = STATE_IN_STDIN;
                setCommandType(token, STDIN);
            }
        }
        else if (tokenType == TOKEN_STDOUT)
        {
            if (isLastToken(iIndex, arrayLength))
            {
                commandresult->error = 1;
                commandresult->errormsg = "Invalid: Standard output redirection without file name\n";
            }
            else
            {
                commandresult->cState = STATE_IN_STDOUT;
                setCommandType(token, STDOUT);
            }
        }
        else if (tokenType == TOKEN_PIPE)
        {
            if (isLastToken(iIndex, arrayLength))
            {
                commandresult->error = 1;
                commandresult->errormsg = "Invalid: Missing command name\n";
            }
            else
            {
                commandresult->cState = STATE_IN_PIPE;
                setCommandType(token, PIPE);
            }
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
            setCommandType(token, ARGUMENT);
        }
        else if (tokenType == TOKEN_STDIN)
        {
            if (isLastToken(iIndex, arrayLength))
            {
                commandresult->error = 1;
                commandresult->errormsg = "Invalid: Standard input redirection without file name\n";
            }
            else
            {
                commandresult->cState = STATE_IN_STDIN;
                setCommandType(token, STDIN);
            }
        }
        else if (tokenType == TOKEN_STDOUT)
        {
            if (isLastToken(iIndex, arrayLength))
            {
                commandresult->error = 1;
                commandresult->errormsg = "Invalid: Standard output redirection without file name\n";
            }
            else
            {
                commandresult->cState = STATE_IN_STDOUT;
                setCommandType(token, STDOUT);
            }
        }
        else if (tokenType == TOKEN_PIPE)
        {
            if (isLastToken(iIndex, arrayLength))
            {
                commandresult->error = 1;
                commandresult->errormsg = "Invalid: Missing command name\n";
            }
            else
            {
                commandresult->cState = STATE_IN_PIPE;
                setCommandType(token, PIPE);
            }
        }
        else
        {
            fprintf(stderr, "should not reach here\n");
        }
        break;

    case STATE_IN_STDIN:
        if (tokenType == TOKEN_WORD)
        {
            commandresult->cState = STATE_IN_FILE;
            commandresult->error = 0;
            commandresult->errormsg = NULL;
            setCommandType(token, FILENAME);
        }
        else
        {
            commandresult->cState = cState;
            commandresult->error = 1;
            commandresult->errormsg = "Invalid: Standard input redirection without input file name\n";
        }
        break;

    case STATE_IN_STDOUT:
        if (tokenType == TOKEN_WORD)
        {
            commandresult->cState = STATE_IN_FILE;
            commandresult->error = 0;
            commandresult->errormsg = NULL;
            setCommandType(token, FILENAME);
        }
        else
        {
            commandresult->cState = cState;
            commandresult->error = 1;
            commandresult->errormsg = "Invalid: Standard input redirection without output file name\n";
        }
        break;

    case STATE_IN_PIPE:
        if (tokenType == TOKEN_WORD)
        {
            commandresult->cState = STATE_IN_COMMAND;
            commandresult->error = 0;
            commandresult->errormsg = NULL;
            setCommandType(token, COMMAND);
        }
        else
        {
            commandresult->cState = cState;
            commandresult->error = 1;
            commandresult->errormsg = "Invalid: No Command after pipe operation\n";
        }
        break;

    case STATE_IN_FILE:
        commandresult->error = 0;
        commandresult->errormsg = NULL;
        if (tokenType == TOKEN_STDIN)
        {
            commandresult->cState = STATE_IN_STDIN;
            setCommandType(token, STDIN);
        }
        else if (tokenType == TOKEN_STDOUT)
        {
            commandresult->cState = STATE_IN_STDOUT;
            setCommandType(token, STDOUT);
        }
        else if (tokenType == TOKEN_PIPE)
        {
            commandresult->cState = STATE_IN_PIPE;
            setCommandType(token, PIPE);
        }
        else if (tokenType == TOKEN_WORD)
        {
            commandresult->cState = STATE_IN_ARGUMENT;
            setCommandType(token, ARGUMENT);
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
    enum TokenType tokenType = getTokenType(token);
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
            ioResult->errormsg = "Invalid: Multiple redirection of standard input\n";
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
    enum ioState ioState = NONE;
    for (iIndex = 0; iIndex < tokenArrayLength; iIndex++)
    {
        struct commandResult *commandresult = commandContext(oTokens, iIndex, tokenArrayLength, cState);
        if (commandresult->error)
        {
            fprintf(stderr, "%s", commandresult->errormsg);
            return FALSE;
        }
        else
        {
            cState = commandresult->cState;
        }
        free(commandresult);

        struct ioResult *ioresult = ioContext(oTokens, iIndex, tokenArrayLength, ioState);
        if (ioresult->error)
        {
            fprintf(stderr, "%s", ioresult->errormsg);
            return FALSE;
        }
        else
        {
            ioState = ioresult->ioState;
        }
        free(ioresult);
    }
    return TRUE;
}

/*
int main(void)
{

    char acLine[MAX_LINE_SIZE];
    int isSuccessful;
    int valid;

    DynArray_T oTokens;

    printf("------------------------------------\n");
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
            DynArray_map(oTokens, printAnyTokenWithTokenType, NULL);
        }
        if (isSuccessful)
        {
            printf("%s\n", "valid");
            DynArray_map(oTokens, printAnyTokenWithCommandType, NULL);
        }
        else
        {
            printf("%s\n", "invalid");
        }
        printf("------------------------------------\n");
        DynArray_map(oTokens, freeToken, NULL);
        DynArray_free(oTokens);
    }
    return 0;
}
*/