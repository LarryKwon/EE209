#ifndef LEX_H
#define LEX_H
#include "dynarray.h"
#include "token.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int lexLine(const char *pcLine, DynArray_T oTokens);
#endif