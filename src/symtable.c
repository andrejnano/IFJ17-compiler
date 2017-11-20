#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "symtable.h"

#define DEF_NODE_STACK_SIZE 20
void STL_init(tSymbolTable **tree) {
	*tree = (tSymbolTable*)malloc(sizeof(tSymbolTable));
	(*tree)->top = NULL;
	(*tree)->next = NULL;
}
/* \brief Function Adds new sybol table to the top of symboltable list
 * \param tree symbol table pointer.
 */
void STL_push(tSymbolTable **tree) {
	tSymbolTable *newST = (tSymbolTable *)malloc(sizeof
	(tSymbolTable));
	newST->next = *tree;
	newST->top = NULL;
	*tree = newST;
}
/* \brief Function for inserting values into binary tree.
 * \param tree symbol table pointer.
 * \param key Identifier of inserted node.
 * \param varType Type of variable in val.
 * \param val Variable with different possible types.
 */
int STL_insert_top(tSymbolTable *tree, char *key, node_val_t *value) {
   if (!tree)
      return 1;
   if (tree->top == NULL) {
      tree->top = (node_t *) malloc(sizeof(node_t));
      memset(tree->top, 0, sizeof(node_t));
      tree->top->key = (char *) malloc(sizeof(char) * strlen(key) + 1);
      strcpy(tree->top->key, key);
	  tree->top->val = (node_val_t *)malloc(sizeof(node_val_t));
	  memcpy(tree->top->val, value, sizeof(node_val_t));
      return 0;
   }
   node_t *tmp = tree->top;
   while (tmp) {
      int cmp = strcmp(tmp->key, key);
      if (!cmp) {
		 memcpy(tmp->val, value, sizeof(node_val_t));
         return 0;
      } else if (cmp > 0) {
         if (!tmp->l_ptr) {
            tmp->l_ptr = (node_t *) malloc(sizeof(node_t));
            memset(tmp->l_ptr, 0, sizeof(node_t));
            tmp = tmp->l_ptr;
            break;
         }
         tmp = tmp->l_ptr;
      } else {
         if (!tmp->r_ptr) {
            tmp->r_ptr = (node_t *) malloc(sizeof(node_t));
            memset(tmp->r_ptr, 0, sizeof(node_t));
            tmp = tmp->r_ptr;
            break;
         }
         tmp = tmp->r_ptr;
      }
   }
   tmp->key = (char *) malloc(sizeof(char) * strlen(key) + 1);
   strcpy(tmp->key, key);
   tmp->val = (node_val_t *)malloc(sizeof(node_val_t));
   memcpy(tmp->val, value, sizeof(node_val_t));
   return 0;
}

/*
 * \brief Function searches binary tree for str key.
 * \param tree Whole tree pointer.
 * \param key Key for comparing.
 */
node_val_t *STL_search(tSymbolTable *tree, char *key) {
	if (!tree) {
		return NULL;
	}
   node_t *tmp = tree->top;
   if (!key)
		return NULL;
   while (tmp) {
      int cmp = strcmp(tmp->key, key);
      if (!cmp) {
         return tmp->val;
      } else if (cmp > 0) {
         tmp = tmp->l_ptr;
      } else {
            tmp = tmp->r_ptr;
         }
      }
   return NULL;
}

typedef struct {
   node_t **arr;
   int idx;
   unsigned len;   
} stack_t;

//TODO node conections print
void print_tree(node_t *t, int i) {
   if (t->l_ptr) {
      int j = i;
      while (j != 0) {
         printf("  ");
         j--;
      }
      print_tree(t->l_ptr, i+1);
   }
   if (t) {
      int j = i;
      while (j != 0) {
         printf("  ");
         j--;
      }
      printf("%s\n", t->key);
   }
   if (t->r_ptr) {
      int j = i;
      while (j != 0) {
         printf("  ");
         j--;
      }
      print_tree(t->r_ptr, i+1);
   }
}

/*
 * \brief Free memory of arglist node by node
 * \param args List of arguments
 */
void DisposeArgList(tArglist *args) {
	while (args) {
		tArglist *tmp = args;
		args = args->next;
		free(tmp->name);
		free(tmp);
	}
	free(args);
}

/*
 * \brief compares two arglists node by node
 * \return 1 if arglists are same 0 otherwise
 */
bool isSameArglists(tArglist *arg1, tArglist *arg2) {
	while (arg1 && arg2) {
		if (strcmp(arg1->name, arg2->name) != 0) {
			return 0;
		}
		if (arg1->type != arg2->type) {
			return 0;
		}
		arg1 = arg1->next;
		arg2 = arg2->next;
      }
      if (!arg1 && !arg2)
            return 1;
      return 0;
}

/*
 * \brief Generates symboltable from arguments of function
 * \param funcName Name of the functio
 * \param functions Table of functions
 * \return Symboltable containing arguments of function
 */
tSymbolTable *argsToSymtable(char *funcName, tSymbolTable *functions)
{
	tSymbolTable *localVars;
	STL_init(&localVars);
	tArglist *argList;
	node_val_t *tmpMeta;
	tmpMeta = STL_search(functions, funcName);
	argList = NULL;
	if (!tmpMeta)
		return NULL;
	argList = tmpMeta->args;

	while (argList)
	{
		node_val_t val;
		val.args = NULL;
		val.type = argList->type;
		STL_insert_top(localVars, argList->name, &val);
		argList = argList->next;
	}
	return localVars;
}

/*
 * \brief Recursive function for cleaning up tree
 * \param top Top of the tree to be cleaned
 */
void DisposeTree(node_t **top) {	
	if (!(*top)) {
		return;
	}
	if (!(*top)->l_ptr && !(*top)->r_ptr) {
		free((*top)->key);
		tArglist *args = (*top)->val->args;
		DisposeArgList(args);
		free((*top)->val);
		free((*top));
		(*top) = NULL;
	} else {
		if ((*top)->l_ptr)
			DisposeTree(&((*top)->l_ptr));
		if ((*top)->r_ptr)
			DisposeTree(&(((*top))->r_ptr));	
		free((*top)->key);
		tArglist *args = (*top)->val->args;
		DisposeArgList(args);
		free((*top)->val);
		free((*top));
	}

}

/*
 * \brief Disposes symboltable from the top of symtable list
 * and pops the list
 * \param tree First simboltable in the list pointer
 * \return 0 on success 1 otherwise
 */
int STL_pop(tSymbolTable **tree) {
	if (!tree || !*tree) {
		return 1;
	}
	DisposeTree(&((*tree)->top));
	tSymbolTable *tmp;
	tmp = (*tree)->next;
	free(*tree);
	*tree = tmp;
	return 0;
}

/*
 * \brief Disposes whole list of symboltables
 * \param tree First simboltable in the list pointer
 */
void STL_clean_up(tSymbolTable **tree) {
	int out;
	while (out != 1) {
		out = STL_pop(tree);
   }
}
