/*
    + ------------- +
    | PROJEKT IFJ17 |
    + ------------- +

    Společný projekt IFJ a IAL 2017
        Implementace prekladace imperativního jazyka IFJ17.

    Varianta zadanie:
        Tým 025, varianta I

    Soubor:
        expression.h

    Autori:
        xmarko15 Peter Marko
        xmechl00 Stanislav Mechl
        xnanoa00 Andrej Nano
        xsvand06 Švanda Jan
*/

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <stdio.h>
#include "scanner.h"
#include "symtable.h"
// structure for operator stack
typedef struct sStack {
   int type; // type of operator token_op_add, token_op_sub ...
   int lOperandType; // data type of left operand
   int priority; // priority of operator higher number less priority
   struct sStack *next; // next operator in stack
} tStack;

#define STACK_STOPPER 100  // special value at the end of operator stack
// classic stac functions
void sInit(tStack **s);
void sPush(tStack **s, tStack *val);
tStack *sPop(tStack **s);

// Generates new unused name for variable or label
void generateName(char **var);
// Generating ifj17code for initialization of new var
void zeroVarInit(char *varName);
// Function for generating code for function call
void NT_CallExpr(Metadata_t *funcMeta, char *funcName);
// Function for generating code for expression evaluation
void NT_Expr(int type);
#endif
