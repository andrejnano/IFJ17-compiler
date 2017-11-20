#ifndef SYM_TABLE
#define SYM_TABLE

#include <stdio.h>
#include <stdbool.h>
#include "token.h"
#include "errors.h"

enum val_type {
   TYPE_INT = token_integer,
   TYPE_STRING = token_string,
   TYPE_BOOLEAN = token_boolean,
   TYPE_DOUBLE = token_double,
};

typedef struct sArglist {
   int type;
   char *name;
   struct sArglist *next;
} tArglist;

typedef struct
{
   int type;
   tArglist *args;
   bool dec;
} node_val_t;
  
typedef struct node_s
{
   char *key;
   node_val_t *val;
   struct node_s *l_ptr;
   struct node_s *r_ptr;
} node_t;

typedef struct sSymbolTable
{
   struct node_s *top;
   struct sSymbolTable *next;
} tSymbolTable;
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "symtable.h"

#define DEF_NODE_STACK_SIZE 20
void STL_init(tSymbolTable **tree);
void STL_push(tSymbolTable **tree);
int STL_insert_top(tSymbolTable *tree, char *key, node_val_t *value);
node_val_t *STL_search(tSymbolTable *tree, char *key);
int STL_pop(tSymbolTable **tree);
void STL_clean_up(tSymbolTable **tree);
void DisposeArgList(tArglist *args);
int argListAppend(tArglist **arglist, tArglist *node);
bool isSameArglists(tArglist *arg1, tArglist *arg2);
tSymbolTable *argsToSymtable(char *funcName, tSymbolTable *functions);

#endif