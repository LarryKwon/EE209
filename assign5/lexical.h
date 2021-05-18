#ifndef LEX_H
#define LEX_H
#include "dynarray.h"
#include "token.h"

int lexLine(const char *pcLine, DynArray_T oTokens);
#endif