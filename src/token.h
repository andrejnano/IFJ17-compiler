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

    // vnutorny konkretny typ tokenu
    typedef enum
    {
        token_blank,
        token_identifier,
        token_integer,
        token_double,
        token_string,
        token_operator,
        token_variable,
        token_semicolon,
        // ... bude ich asi urcite viac, neviem ale ake vsetky chceme,
    } token_type;

    typedef union
    {
        char *c;
        double d;
        int i;
    } attribute_value;

    // datovy typ pre objekt 'token'
    typedef struct _TOKEN
    {
        token_type type;
        attribute_value att;
    } Token_t;

    /*
        Token_t mytoken;
        mytoken.type = token_identifier;
        mytoken.value = "count";
        mytoken.size = strlen(mytoken.value); ? ? ?
    */
#endif
