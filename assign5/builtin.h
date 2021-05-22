#ifndef BUILTIN_H
#define BUILTIN_H

enum BuiltInCommand
{
    CD,
    SETENV,
    UNSETENV,
    EXIT,
    NOTHING
};

typedef int (*BuiltIn_T)(DynArray_T);

int execCd(DynArray_T oTokens);

int execSetenv(DynArray_T oTokens);

int execUnsetenv(DynArray_T oTokens);

int execExit(DynArray_T oTokens);

enum BuiltInCommand isBuiltIn(DynArray_T oTokens);

BuiltIn_T BuiltInContext(enum BuiltInCommand builtInCommand);

#endif