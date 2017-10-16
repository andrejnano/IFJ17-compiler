#ifndef SYM_TABLE
#define SYM_TABLE

#include <stdio.h>

enum val_type {
   TYPE_INT,
   TYPE_CHAR,
   TYPE_DOUBLE,
};

typedef union
{
   char c;
   double d;
   int i;
} val_union;

typedef struct
{
   int type;  // type of value in union
   val_union value;
} node_val_t;
  
typedef struct node_s
{
   char *key;
   node_val_t val;
   struct node_s *l_ptr;
   struct node_s *r_ptr;
} node_t;

typedef struct
{
   struct node_s *top;
} b_tree;

void b_tree_init(b_tree *tree);
int b_tree_insert(b_tree *tree, char *key, int varType, val_union val);
node_val_t *b_tree_search(b_tree *tree, char *key);
void b_tree_free(node_t *tree);

#endif