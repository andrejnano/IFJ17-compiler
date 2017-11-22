#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <stdio.h>
#include "scanner.h"
#include "symtable.h"

typedef struct sStack {
   int type;
   int lOperandType;
   int priority;
   struct sStack *next;
} tStack;

#define STACK_STOPPER 100
void sInit(tStack **s);
void sPush(tStack **s, tStack *val);
tStack *sPop(tStack **s);

void generateName(char **var);
void zeroVarInit(char *varName);
void NT_Func(bool dec, char **funcName);
void NT_CallExpr(Metadata_t *funcMeta, char *funcName, SymbolTable_t *localVars);
void NT_Expr(int type, SymbolTable_t *localVars);
char *convert(int inType, int outType, token_value val, char *frame);
#endif