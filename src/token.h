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

    typedef enum
    {
        err_lex, // 1 - chybna struktura aktualniho lexemu
        err_syn, // 2 - chybna syntaxe
        err_sem_def, // 3 - nedefinova funkce/promena nebo pokus o redefinici
        err_sem_com, // 4 - nekompatibilni typy nebo spatne parametry funkce
        err_sem_oth, // 6 - ostatni semanticke chyby
        err_int, // 99 - interni chyba prekladace (neovlivnena vstupnim programem)
    } error_type;

    typedef union
    {
        char *c;
        double d;
        int i;
        error_type e;
    } attribute_value;

    // datovy typ pro objekt 'token'
    // pokud mame ukazatel pT ukazujici na token,
    // k typu pristoupime jako pT->type
    // a k atributu jako pT->att.c  (pripadne i, d nebo e)
    // pozor! c je ukazatel! pro string ok pro pristup ke znaku *(pT->att.c)
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
