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

    // Token type, identifies token
    typedef enum
    {
        token_blank,
        token_error,
        token_identifier,
        token_integer,
        token_double,
        token_string,
        token_operator,
        token_variable,
        token_semicolon,
        // ... bude ich asi urcite viac, neviem ale ake vsetky chceme,
    } token_type;

    // Token values
    typedef union
    {
        char *c;
        double d;
        int i;
    } token_value;

    // Token error values
    typedef enum
    {
        token_success, // Correct token
        token_err_string_term, // String not terminated correctly
        token_err_string_esc, // Wrong escape sequence or character value
        token_err_conversion, // Unsuccessful conversion (int, double)
        token_err_eof_unexpected, // Unexpected end of file
    } token_error;

    // datovy typ pro objekt 'token'
    // pokud mame ukazatel pT ukazujici na token,
    // k typu pristoupime jako pT->type
    // a k atributu jako pT->att.c  (pripadne i, d nebo e)
    // pozor! c je ukazatel! pro string ok pro pristup ke znaku *(pT->att.c)
    typedef struct _TOKEN
    {
        token_type type;
        token_value value;
        token_error err;
        unsigned int line;
        unsigned int col;
    } Token_t;

    /*
        Token_t *mytoken;
        mytoken = get_token(file);

        mytoken->type = token_identifier;
        mytoken->value.i = 1234;
        mytoken->err = token_success
        mytoken->line = 3; // <1, inf)
        mytoken->col = 1; // <1, inf)
    */

#endif
