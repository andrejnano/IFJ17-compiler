#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "symtable.h"


/*
 * @TODO list : pridat limit pre alokaciu Key na max velkost identifikatora
 *
 */

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
        memcpy(tree->top->metadata, new_data, sizeof(Metadata_t));

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
            memcpy(current_item->metadata, new_data, sizeof(Metadata_t));
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
    current_item->metadata = (Metadata_t *) malloc(sizeof(Metadata_t));
    memcpy(current_item->metadata, new_data, sizeof(Metadata_t));

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


/*
 * \brief Free memory of arglist node by node
 * \param args List of arguments
 */
void param_list_dispose(Parameter_t *parameter)
{
    Parameter_t *tmp;
    
    while (parameters != NULL)
    {
        tmp = parameters;
        parameters = parameters->next;
        free(tmp->name);
        free(tmp);
    }
}

/*
 * \brief compares two parameters lists node by node
 * \return true if they are same false otherwise
 */
bool param_list_cmp(Parameter_t *p1, Parameter_t *p2)
{
    while (p1 != NULL && p2 != NULL)
    {
        if (strcmp(p1->name, p2->name) != 0)
            return false;
        
        if (p1->type != p2->type)
            return false;

        p1 = p1->next;
        p2 = p2->next;
    }

    if (p1 == NULL && p2 == NULL)
        return true;
    else
        return false;
}

/*
 * \brief Append new node to parameter list
 * \param parameter_list Top of the list
 * \param parameter parameter to be inserted
 * \return true if success, false if failed
 */
bool param_list_append(Parameter_t **parameter_list, Parameter_t *new_parameter)
{
    Parameter_t *prev_parameter = NULL;
    Parameter_t *current_parameter = (*parameter_list);

    if (current_parameter == NULL)
    {
        (*parameter_list) = new_parameter;
        return true;
    }

    while (current_parameter != NULL)
    {

        // if there are 2 same parameters
        if ( strcmp(current_parameter->name, new_parameter->name) == 0 )
        {
            raise_error(SEM_ERROR, "Same named parameters in function declaration\n");
            return false;
        }

        prev_parameter = current_parameter;
        current_parameter = current_parameter->next;
    }
    
    prev_parameter->next = new_parameter;

    return 0;
}

/*
 * \brief Generates symbol table from parameters of function
 * \param funcName Name of the functio
 * \param functions Table of functions
 * \return Symboltable containing arguments of function
 */
SymbolTable_t *param_to_symtable(char *func_name, SymbolTable_t *functions)
{
    SymbolTable_t *local_vars;
    stl_init(&local_vars); // @TODO Kde sa toto cisti ??? 

    // find the function in symtable
    Metadata_t *metadata_query = STL_search(functions, func_name);
    
    if (metadata_query == NULL)
        return NULL;


    // find function's parameters
    Parameter_t *parameters = metadata_query->parameters;

    // for each parameter create new Item in symtable
    while (parameters != NULL)
    {
        // create Metadata for each item
        Metadata_t metadata;
        metadata.parameters = NULL;
        metadata.is_declared = true; // probably?
        metadata.is_defined = false;
        metadata.type = parameters->type;
        
        stl_insert_top(local_vars, parameters->name, &metadata);
        parameters = parameters->next;
    }

    return local_vars;
}

/*
 * \brief Recursive function for cleaning up tree
 * \param top Top of the tree to be cleaned
 */
void tree_dispose(SymbolTable_t *st)
{
    Item_t current_item = st->top;

    if ( st->top == NULL )
    {
        return;
    }

    if (current_item->left_ptr == NULL && current_item->right_ptr == NULL)
    {
        free(current_item->key);
        param_list_dispose(current_item->metadata->parameters);
        free(current_item->metadata);
        free(current_item);
        current_item = NULL;
    }
    else
    {
        if (current_item->left_ptr)
            tree_dispose(current_item->left_ptr);
        if (current_item->right_ptr)
            tree_dispose(current_item->right_ptr);
        
        free(current_item->key);
        param_list_dispose(current_item->metadata->parameters);
        free(current_item->metadata);
        free(current_item);
    }
}

/*
 * \brief Disposes symboltable from the top of symtable list
 * and pops the list
 * \param tree First symboltable in the list pointer
 * \return true on success false on fail
 */
bool stl_pop(SymbolTable_t **tree)
{
    if (tree == NULL || (*tree) == NULL)
    {
        return false;
    }

    tree_dispose((*tree));
    SymbolTable_t *tmp = (*tree)->next;
    free(*tree);
    *tree = tmp;

    return true;
}

/*
 * \brief Disposes whole list of symboltables
 * \param tree First simboltable in the list pointer
 */
void stl_clean_all(SymbolTable_t **tree)
{
      int out;
      while (out != 1)
      {
          out = STL_pop(tree);
      }
}

/**************************************************/
/**************************************************/

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