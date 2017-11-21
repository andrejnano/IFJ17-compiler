#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "symtable.h"

#define DEF_NODE_STACK_SIZE 20

/* 
 * @brief Function Initializes symbol table list
 * @param tree symbol table pointer.
 */
void stl_init(SymbolTable_t **tree)
{
    *tree = (SymbolTable_t *) malloc(sizeof(SymbolTable_t));
    (*tree)->top = NULL;
    (*tree)->next = NULL;
}


/* 
 * @brief Function Adds new sybol table to the top of symboltable list
 * @param tree symbol table pointer.
 */
void stl_push(SymbolTable_t **tree)
{
    SymbolTable_t *new_ST = (SymbolTable_t *) malloc(sizeof(SymbolTable_t));
    new_ST->next = (*tree);
    new_ST->top = NULL;
    *tree = newST;
}


/* @brief Function for inserting values into binary tree.
 * @param tree symbol table pointer.
 * @param key Identifier of inserted node.
 * @param value 
 * @return bool TRUE if successful, FALSE if failed
 */
bool stl_insert_top(SymbolTable_t *tree, char *key, Metadata_t *new_data)
{

    if (!tree)
        return false;

    // if ST List is empty
    if (tree->top == NULL)
    {
        // create new Symbol Table Item
        tree->top = (Item_t *) malloc(sizeof(Item_t));
        memset(tree->top, 0, sizeof(Item_t));

        // create new Key for the Item
        tree->top->key = (char *) malloc(sizeof(char) * strlen(key) + 1);
        strcpy(tree->top->key, key);

        // create new Metadata for the Item
        tree->top->data= (Metadata_t *) malloc(sizeof(Metadata_t));
        memcpy(tree->top->data, new_data, sizeof(Metadata_t));

        return true;
    }


    Item_t *current_item = tree->top;
    int key_difference;

    while (current_item != NULL)
    {

        key_difference = strcmp(key, current_item->key);

        // update metadata of an already existing item and end
        if (key_difference == 0)
        {
            memcpy(current_item->data, new_data, sizeof(Metadata_t));
            return true; 
        }

        // arg. key is greater than current key
        else if (key_difference > 0) 
        {
            if (current_item->right_ptr == NULL)
            {
                current_item->right_ptr = (Item_t *) malloc( sizeof(Item_t) );
                memset(current_item->right_ptr, 0, sizeof(Item_t));
                current_item = current_item->right_ptr;
                break;
            }
            
            current_item = current_item->right_ptr; // move
        }

        // arg. key is smaller than current key
        else if (key_difference < 0) 
        {
            if (current_item->left_ptr == NULL)
            {
                current_item->left_ptr = (Item_t *) malloc(sizeof(Item_t));
                memset(current_item->left_ptr, 0, sizeof(Item_t));
                current_item = current_item->left_ptr;
                break;
            }
            current_item = current_item->left_ptr; // move
        }
    }

    // create new Key for the Item
    current_item->key = (char *) malloc(sizeof(char) * strlen(key) + 1);
    strcpy(current_item->key, key);

    // create new Metadata for the Item
    current_item->data = (Metadata_t *) malloc(sizeof(Metadata_t));
    memcpy(current_item->data, new_data, sizeof(Metadata_t));

    return true;
}

/*
 * \brief Function searches binary tree for str key.
 * \param tree Whole tree pointer.
 * \param key Key for comparing.
 */
Metadata_t *stl_search(SymbolTable_t *tree, char *key)
{
    if (!tree || !key)
    {
        return NULL;
    }

    node_t *current_item = tree->top;
    int key_difference;

    while (current_item != NULL)
    {
        key_difference = strcmp(key, tmp->key);
        
        if (key_difference == 0)
        {
            return current_item->data;
        }
        else if (key_difference > 0)
        {
            current_item = current_item->right_ptr;
        }
        else
        {
            current_item = current_item->left_ptr;
        }
    }

    return NULL;
}

typedef struct
{
      node_t **arr;
      int idx;
      unsigned len;
} stack_t;

//TODO node conections print
void print_tree(node_t *t, int i)
{
      if (t->l_ptr)
      {
            int j = i;
            while (j != 0)
            {
                  printf("  ");
                  j--;
            }
            print_tree(t->l_ptr, i + 1);
      }
      if (t)
      {
            int j = i;
            while (j != 0)
            {
                  printf("  ");
                  j--;
            }
            printf("%s\n", t->key);
      }
      if (t->r_ptr)
      {
            int j = i;
            while (j != 0)
            {
                  printf("  ");
                  j--;
            }
            print_tree(t->r_ptr, i + 1);
      }
}

/*
 * \brief Free memory of arglist node by node
 * \param args List of arguments
 */
void param_list_clear(Parameter_t *parameters)
{
    Parameter_t *tmp
    
    while (parameters != NULL)
    {
        tmp = parameters;
        parameters = parameters->next;
        free(tmp->name);
        free(tmp);
    }
}

/*
 * \brief compares two arglists node by node
 * \return 1 if arglists are same 0 otherwise
 */
bool param_cmp(Parameter_t *parameters, Parameter_t *parameters)
{
      while (arg1 && arg2)
      {
            if (strcmp(arg1->name, arg2->name) != 0)
            {
                  return 0;
            }
            if (arg1->type != arg2->type)
            {
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
 * \brief Append new node to argument list
 * \param arglist Top of the list
 * \param node node to be inserted
 * \return true at succes, 0 otherwise
 */
bool param_list_append(tArglist **arglist, tArglist *node)
{
      tArglist *prev = NULL;
      tArglist *tmp = *arglist;
      if (!tmp)
      {
            *arglist = node;
            return 0;
      }
      while (tmp)
      {
            if (tmp->name && strcmp(tmp->name, node->name) == 0)
            {
                  raise_error(SEM_ERROR, "Same named arguments in function declaration\n");
                  return 1;
            }
            prev = tmp;
            tmp = tmp->next;
      }
      if (prev)
            prev->next = node;
      return 0;
}

/*
 * \brief Generates symboltable from arguments of function
 * \param funcName Name of the functio
 * \param functions Table of functions
 * \return Symboltable containing arguments of function
 */
SymbolTable_t *params_to_symtable(char *func_name, SymbolTable_t *functions)
{
    SymbolTable_t *local_vars;
    stl_init(&local_vars);


    Metadata_t *metadata_query = STL_search(functions, func_name);
    if (metadata_query == NULL)
        return NULL;

    Parameter_t *parameters;

    parameters = ->args;

    while (argList)
    {
        node_val_t val;
        val.args = NULL;
        val.dec = false;
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
void DisposeTree(node_t **top)
{
      if (!(*top))
      {
            return;
      }
      if (!(*top)->l_ptr && !(*top)->r_ptr)
      {
            free((*top)->key);
            tArglist *args = (*top)->val->args;
            DisposeArgList(args);
            free((*top)->val);
            free((*top));
            (*top) = NULL;
      }
      else
      {
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
int STL_pop(tSymbolTable **tree)
{
      if (!tree || !*tree)
      {
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
void STL_clean_up(tSymbolTable **tree)
{
      int out;
      while (out != 1)
      {
            out = STL_pop(tree);
      }
}
