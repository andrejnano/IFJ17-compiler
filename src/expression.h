#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <stdio.h>
#include "scanner.h"
#include "symtable.h"
#include "parser.h"

typedef struct sStack {
   int type;
   int op1p;
   int priority;
   struct sStack *next;
} tStack;

#define STACK_STOPPER 100
void sInit(tStack **s);
void sPush(tStack **s, tStack *val);
tStack *sPop(tStack **s);

void generateName(char **var);
void zeroVarInit(char *varName);
void ntFunc(bool dec, char **funcName);
void ntCallExpr(node_val_t *funcMeta, char *funcName, tSymbolTable *localVars);
void ntExpr(int type, tSymbolTable *localVars);
char *convert(int inType, int outType, token_value val, char *frame);
#endif