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
#include <inttypes.h>
#include <stdio.h>
#include <stdbool.h>
    // Token type, identifies token
    typedef enum
    {

    	// Data types
        token_blank,
        token_identifier,
        token_val_integer,
        token_val_double,
        token_val_string,

        // Operators
        token_op_add,
        token_op_sub,
        token_op_mul,
        token_op_div,
        token_op_mod,
        token_op_lt,
        token_op_gt,
        token_op_le,
        token_op_ge,
        token_op_eq,
        token_op_ne,

        // Special
        token_semicolon,
        token_comma,
        token_lbrace,
        token_rbrace,

        // Keywords
        token_and,
        token_as,
        token_asc,
        token_boolean,
        token_chr,
        token_continue,
        token_declare,
        token_dim,
        token_do,
        token_double,
        token_else,
        token_elseif,
        token_end,
        token_exit,
        token_false,
        token_for,
        token_function,
        token_if,
        token_input,
        token_integer,
        token_length,
        token_loop,
        token_next,
        token_not,
        token_or,
        token_print,
        token_return,
        token_scope,
        token_shared,
        token_static,
        token_string,
        token_substr,
        token_then,
        token_true,
        token_while,

        // Read state
        token_eol,
        token_eof,

    } token_type;

    // Token values
    typedef union
    {
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

    /*
        Token_t *mytoken;
        mytoken = get_token(file);
        mytoken->type = token_identifier;
        mytoken->value.i = 1234;
        mytoken->line = 3; // <1, inf)
    */

extern Token_t active_token;
extern FILE *input, *output;

  bool istype(int type);
  void printTokenType(FILE *output_file, int type);
  bool match(int token_type, int expected_token_type);
  void printTokenVal(void);
#endif
    