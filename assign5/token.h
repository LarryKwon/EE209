#ifndef TOKEN_INCLUDED
#define TOKEN_INCLUDED
enum TokenType
{
    TOKEN_WORD,
    TOKEN_STDIN,
    TOKEN_STDOUT,
    TOKEN_PIPE
};

enum CommandType
{
    COMMAND,
    ARGUMENT,
    STDIN,
    STDOUT,
    PIPE,
    FILENAME
};

/* A Token is either a number or a word, expressed as a string. */
/* The type of the token. */
/* The string which is the token's value. */
typedef struct Token *Token_T;

/* Create and return a Token whose type is eTokenType and whose
   value consists of string pcValue.  Return NULL if insufficient
   memory is available.  The caller owns the Token. */
Token_T makeToken(enum TokenType eTokenType, char *pcValue);

/* get TokenType in the Token */
enum TokenType getTokenType(Token_T token);

/* get TokenValue in the Token */
char *getTokenValue(Token_T token);

/* set CommandType in the token */
void setCommandType(Token_T token, enum CommandType commandType);

/*get CommandType in the token*/
enum CommandType getCommandType(Token_T token);

/* Print token pvItem to stdout iff it is a word.  pvExtra is
   unused. */
void printWordToken(void *pvItem, void *pvExtra);

/*print token pvItem to stdout iff it is a string. pvExtra is 
    unused. */
void printStringToken(void *pvItem, void *pvExtra);

/*print token pvItem to stdout which token type is any kind of thing
with tokenType 
pvExtra is unused. */
void printAnyTokenWithTokenType(void *pvItem, void *pvExtra);

/*print token pvItem to stdout which token type is any kind of thing
with commandType 
pvExtra is unused. */
void printAnyTokenWithCommandType(void *pvItem, void *pvExtra);

/* Free token pvItem.  pvExtra is unused. */
void freeToken(void *pvItem, void *pvExtra);

#endif