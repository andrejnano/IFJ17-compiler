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

    Zadanie:
        Vytvorte program v jazyce C, který nacte zdrojový kód zapsaný ve zdrojovém
        jazyce IFJ17 a preloží jej do cílového jazyka IFJcode17 (mezikód).

*/

#include <stdio.h>
#include <stdlib.h>
#include "errorcodes.h"
#include "token.h"
#include "parser.h"

int main(int argc, char* argv[])
{

    // check arguments, input file @TODO make sepparate function for argument parsing
    if (argc != 2)
        return E_INTERNAL;

    FILE* source_code;  // make global ?

    if (! (source_code = fopen(argv[1], "r")))
        return E_INTERNAL;

    scanner_init();
    parse(source_code, stdout);
    // Token_t *token;
    // token = get_next_token(source_code);
    // while (token->type != token_eof) {
    //     printf("token %s %d\n", token->value.c, token->type);
    //     token = get_next_token(source_code);
    // }
    /* structure of the program
        option 1: call parse function, which inside calls scanner pointed to a global file variable.
        option 2: call scanner with pointer to token vector as argument, call parser with updated token vector
*/


    return SUCCESS;
}
