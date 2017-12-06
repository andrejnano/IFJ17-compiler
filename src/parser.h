/*
    + ------------- +
    | PROJEKT IFJ17 |
    + ------------- +

    Společný projekt IFJ a IAL 2017
        Implementace prekladace imperativního jazyka IFJ17.

    Varianta zadanie:
        Tým 025, varianta I

    Soubor:
        parser.h

    Autori:
        xmarko15 Peter Marko
        xmechl00 Stanislav Mechl
        xnanoa00 Andrej Nano
        xsvand06 Švanda Jan
*/

#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include "symtable.h"
/******************************************************************************
    MAIN STARTING POINT
    - calls the first non-terminal and checks for the required EOL token
    - generates first set of instructions
    - creates global symbol table
    - ...
 *****************************************************************************/

    void parse();

/******************************************************************************
    DECLARATIONS of FUNCTIONS FOR EVERY NON-TERMINAL
    - check if token on input is the expected one
    - or calls another non-terminal function
    - @TODO somehow set error flag if syntax is not valid, however dont stop
            the program from running, display errors at the end.
    - @TODO if the nonterminal has a local scope, then create new symbol table
            instance and semantically check from this table.
            (otherwise use global table)
 *****************************************************************************/

    void NT_Program();
    void NT_Head();
    void NT_Scope();
    void NT_CompoundStmt();
    void NT_FunctionDec();
    void NT_FunctionDef();
    void NT_ParameterList();
    void NT_ParamList();
    void NT_Param();
    void NT_NextParam();
    char *NT_VarDec();
    void NT_VarDef();
    void NT_AssignStmt();
    void NT_IfStmt();
    void NT_WhileStmt();
    void NT_InputStmt();
    void NT_PrintStmt();
    void NT_ReturnStmt();
    void NT_TermList();
    void NT_ExprList();
    void NT_NextExpr();
    void NT_CallExpr();

#endif
