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
    // scan(source_code);
    // parse();

    // tests of symtable
    val_union val, wal;
    val.i = 43;
    b_tree b;
    b_tree_init(&b);
    b_tree_insert(&b, "d", 1, val);
    wal.i = 42;
    b_tree_insert(&b, "b", 1, wal);
    wal.i = 46;
    b_tree_insert(&b, "c", 1, wal);
    wal.i = 49;
    b_tree_insert(&b, "r", 1, wal);

    node_val_t *out = b_tree_search(&b, "r");
    printf("nasiel som %d\n", out->value.i);
    b_tree_free(b.top);

    return SUCCESS;
}
