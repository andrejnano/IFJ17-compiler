/*
    + -------------- +
    | PROJEKT IFJ17  |
    + -------------- +
    Implementace prekladace imperativního jazyka IFJ17.

    Autori:
        xmarko15 Peter Marko
        xmechl00 Stanislav Mechl
        xnanoa00 Andrej Nano
        xsvand06 Švanda Jan
*/

#ifndef PARSER_H
#define PARSER_H

/******************************************************************************
    TOKEN MATCH TEST
    - just checks if active_token is the expected token in the syntax
        > returns TRUE if the token is valid
        > returns FALSE if the tokens is not valid
    - doesn't move to the next token, just informs
    - set of predefined conditions based of LL(1) grammar
 *****************************************************************************/

    bool match_test(token_type expected_token_type);


/******************************************************************************
    TOKEN MATCH & MOVE
    - same as TOKEN MATCH TEST but moves the active token to the next token
    from input with get_token()
 *****************************************************************************/

    bool match_move(token_type expected_token_type);


/******************************************************************************
    MAIN STARTING POINT
    - calls the first non-terminal and checks for the required EOL token
    - generates first set of instructions
    - creates global symbol table
    - ...
 *****************************************************************************/

    int parse();


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

    int NT_program();
    int NT_head();
    int NT_scope();
    int NT_body();
    int NT_func_dec();
    int NT_func_def();
    int NT_func();
    int NT_parameter_list();
    int NT_nextparam();
    int NT_var_dec();
    int NT_var_def();
    int NT_statement();
    int NT_condition();
    int NT_loop();
    int NT_assign();
    int NT_return();
    int NT_callfunc();
    int NT_term_list();
    int NT_next_term();
    int NT_term();


#endif
