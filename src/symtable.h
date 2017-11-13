#ifndef SYM_TABLE
#define SYM_TABLE

#include <stdio.h>
#include <stdbool.h>
#include "token.h"

#define TYPE_DIFF 100
enum val_type {
   TYPE_INT = token_integer,
   TYPE_STRING = token_string,
   TYPE_BOOLEAN = token_boolean,
   TYPE_DOUBLE = token_double,
   TYPE_FUNCTION_INT = TYPE_INT + TYPE_DIFF,
   TYPE_FUNCTION_BOOL = TYPE_STRING + TYPE_DIFF,
   TYPE_FUNCTION_DOUBLE = TYPE_BOOLEAN + TYPE_DIFF,
   TYPE_FUNCTION_STRING = TYPE_DOUBLE + TYPE_DIFF
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

typedef struct
{
   struct node_s *top;
} tSymbolTable;

void ST_init(tSymbolTable *tree);
int ST_insert(tSymbolTable *tree, char *key, node_val_t *varType);
node_val_t *ST_search(tSymbolTable *tree, char *key);
void ST_free(node_t *tree);
void print_tree(node_t *t, int i);

#endif