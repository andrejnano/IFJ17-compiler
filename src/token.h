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

// TOKEN DATA STRUCTURE

#ifndef TOKEN_H
#define TOKEN_H

    #include <stdbool.h>
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>

    // Token type, identifies token
    typedef enum {

        // Data types
        token_blank,        // 0
        token_identifier,   // 1
        token_val_integer,  // 2
        token_val_double,   // 3
        token_val_string,   // 4

        // Operators
        token_op_add,       // 5
        token_op_sub,       // 6
        token_op_mul,       // 7
        token_op_div,       // 8
        token_op_mod,       // 9
        token_op_lt,        // 10
        token_op_gt,        // 11
        token_op_le,        // 12
        token_op_ge,        // 13
        token_op_eq,        // 14
        token_op_ne,        // 15

        // Special
        token_semicolon,    // 16
        token_comma,        // 17
        token_lbrace,       // 18
        token_rbrace,       // 19

        // Keywords
        token_and,          // 20
        token_as,           // 21
        token_asc,          // 22
        token_boolean,      // 23
        token_chr,          // 24
        token_continue,     // 25
        token_declare,      // 26
        token_dim,          // 27
        token_do,           // 28
        token_double,       // 29
        token_else,         // 30
        token_elseif,       // 31
        token_end,          // 32
        token_exit,         // 33
        token_false,        // 34
        token_for,          // 35
        token_function,     // 36
        token_if,           // 37
        token_input,        // 38
        token_integer,      // 39
        token_length,       // 40
        token_loop,         // 41
        token_next,         // 42
        token_not,          // 43
        token_or,           // 44
        token_print,        // 45
        token_return,       // 46
        token_scope,        // 47
        token_shared,       // 48
        token_static,       // 49
        token_string,       // 50
        token_substr,       // 51
        token_then,         // 52
        token_true,         // 53
        token_while,        // 54

        // Read state
        token_eol,          // 55
        token_eof,          // 56

    } token_type;

    // Token values
    typedef union {
        char *c;
        double d;
        int i;
        } token_value;


        // Data structure for token
        // To access its atributes use token_pointer->type
        // tokens with value such as integer, double and string have also their values
        // to access the value use token_pointer->value.i | (i)nteger, (d)ouble, (c)string
        // string (value.c) is an array of char values ending with \0 char
        typedef struct _TOKEN
        {
            token_type type;
            token_value value; // Only if token type is valid with value
            unsigned int line; // On which line is the token located
        } Token_t;
        extern Token_t* active_token;

        bool is_datatype(int type);
        /*
            Token_t *mytoken;
            mytoken = get_token(file);
            mytoken->type = token_identifier;
            mytoken->value.i = 1234;
            mytoken->line = 3; // <1, inf)
        */


    const char* debug_token_name(Token_t *token);


extern int compiler_error;
extern FILE* source_code, *output_code;
    bool istype(int type);
    void printTokenType(FILE *output_file, int type);

/******************************************************************************
    TOKEN MATCH 
    - checks if active_token is the expected token in the syntax
        > returns TRUE if the token is valid
        > returns FALSE if the tokens is not valid
    - moves to the next token
    - for checking a set of predefined conditions based of LL(1) grammar
 *****************************************************************************/

    bool match(token_type expected_token_type);
    void printTokenVal(void);
#endif
