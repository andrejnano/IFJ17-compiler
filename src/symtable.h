
#ifndef SYM_TABLE
#define SYM_TABLE

    #include <stdio.h>
    #include <stdbool.h>
    #include "token.h"


    typedef struct Parameter
    {
        int type;
        char *name;
        struct Parameter *next;
    } Parameter_t;

    typedef struct Metadata
    {
        int type;
        Parameter_t *parameters;
        bool is_defined;
        bool is_declared;
    } Metadata_t;

    typedef struct Item 
    {
        char *key;
        struct Metadata *metadata;
        struct Item *left_ptr;
        struct Item *right_ptr;
    } Item_t;

    typedef struct SymbolTable
    {
        Item_t *top;
        struct SymbolTable *next;
    } SymbolTable_t;

    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include "symtable.h"

    #define DEF_NODE_STACK_SIZE 20

    /** 
     * @brief Function Initializes symbol table list
     * @param tree symbol table pointer.
     */
    void stl_init(SymbolTable_t **tree);

    /** 
     * @brief Function Adds new sybol table to the top of symboltable list
     * @param tree symbol table pointer.
     */
    void stl_push(SymbolTable_t **tree);

    /**
     * @brief Function for inserting values into binary tree.
     * @param tree symbol table pointer.
     * @param key Identifier of inserted node.
     * @param value 
     * @return bool TRUE if successful, FALSE if failed
     */
    bool stl_insert_top(SymbolTable_t *tree, char *key, Metadata_t *new_data);

    /**
     * @brief Function searches binary tree for str key.
     * @param tree Whole tree pointer.
     * @param key Key for comparing.
     */
    Metadata_t *stl_search(SymbolTable_t *tree, char *key);

    /**
     * @brief Free memory of arglist node by node
     * @param args List of arguments
     */
    void param_list_dispose(Parameter_t *parameter);

    /**
     * @brief compares two parameters lists node by node
     * @return true if they are same false otherwise
     */
    bool param_list_cmp(Parameter_t *p1, Parameter_t *p2);

    /**
     * @brief Append new node to parameter list
     * @param parameter_list Top of the list
     * @param parameter parameter to be inserted
     * @return true if success, false if failed
     */
    bool param_list_append(Parameter_t **parameter_list, Parameter_t *new_parameter);

    /**
     * @brief Generates symbol table from parameters of function
     * @param funcName Name of the functio
     * @param functions Table of functions
     * @return Symboltable containing arguments of function
     */
    SymbolTable_t *param_to_symtable(char *func_name, SymbolTable_t *functions);

    /**
     * @brief Recursive function for cleaning up tree
     * @param top Top of the tree to be cleaned
     */
    void tree_dispose(Item_t *current_item);

    /**
     * \brief Checks if all functions in symtable were defined
     * \param current_item the item on which the check will 
     *  recursively start on
     */
    void items_definition_check(Item_t *current_item);

    /**
     * @brief Disposes symboltable from the top of symtable list
     * and pops the list
     * @param tree First symboltable in the list pointer
     * @return true on success false on fail
     */
        bool stl_pop(SymbolTable_t **tree);

    /**
     * @brief Disposes whole list of symboltables
     * @param tree First simboltable in the list pointer
     */
    void stl_clean_all(SymbolTable_t **tree);

extern SymbolTable_t *functions;
extern SymbolTable_t *variables;

#endif
