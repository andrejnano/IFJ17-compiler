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
#include "symtable.h"

int main(int argc, char* argv[])
{

    // check arguments, input file @TODO make sepparate function for argument parsing
    /* if (argc != 2)
        return E_INTERNAL;

    FILE* source_code;  // make global ?

    if (! (source_code = fopen(argv[1], "r")))
        return E_INTERNAL;*/

    /* structure of the program
        option 1: call parse function, which inside calls scanner pointed to a global file variable.
        option 2: call scanner with pointer to token vector as argument, call parser with updated token vector
*/

    /*/ Scanner test start
    printf("Scanner test\n\n");

    // Token
    Token_t *token;

    // Initialize
    if (!scanner_init()) return E_INTERNAL;

    // Read all tokens
    while (token = get_next_token(source_code))
    {

        // Print token info
        printf("Token type %d @ line %d", token->type, token->line);

        // Print token value
        switch (token->type)
        {
            case token_identifier:
            case token_val_string:
                printf(" value |%s|", token->value.c);
                free(token->value.c);
                break;
            case token_val_integer:
                printf(" value %d", token->value.i);
                break;
            case token_val_double:
                printf(" value %f", token->value.d);
                break;
        }

        // Finish the line
        printf("\n");

        // Check end of file
        if (token->type == token_eof)
        {
            free(token);
            break;
        }

        // Free token
        free(token);

    }

    // Close file 
    fclose(source_code);
    
    // Scanner test end */
    
    // parse();

    // tests of symtable
    val_union val, wal;
    val.i = 43;
    b_tree b;
    b_tree_init(&b);
    b_tree_insert(&b, "d", 1, val);
    wal.i = 42;
    b_tree_insert(&b, "b", 1, wal);
    wal.i = 43;
    b_tree_insert(&b, "b", 1, wal);
    wal.i = 46;
    b_tree_insert(&b, "c", 1, wal);
    wal.i = 49;
    b_tree_insert(&b, "r", 1, wal);

    node_val_t *out = b_tree_search(&b, "b");
    printf("nasiel som %d\n", out->value.i);
    b_tree_free(b.top);

    return SUCCESS;
}
