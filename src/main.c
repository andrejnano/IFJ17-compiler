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
    #include "parser.h"

    extern FILE* source_code;
    FILE* output_code;
    char* source_file_name;

    extern int compiler_error;

/******************************************************************************
    MAIN FUNCTION of the IFJ17 COMPILER
    - checks arguments
    - setups environment / source code file / input,output buffer/ symtable,...
    - start parser (syntax analyzer)
    - returns SUCESS or ERROR CODE
    - cleanup, close file, etc..

    @TODO how to output instruction code after compiling ???
 *****************************************************************************/

    int main(int argc, char* argv[])
    {
        
        // check arguments, input file @TODO make sepparate function for argument parsing
        if (argc != 3)
            return E_INTERNAL;

        if (! (source_code = fopen(argv[1], "r")))
            return E_INTERNAL;

        if (!(output_code = fopen(argv[2], "r")))
            return E_INTERNAL;

        // save the source name
        source_file_name = argv[1];

        printf("\n");
        printf("\n");
        raise_error(E_SYNTAX, "test sprava");
        printf("\n");
        raise_error(E_SEM_DEF, "...");
        printf("\n");
        //scanner_init();
        //parse(); // uses globals source_code & output_code

        //cleanup();
        fclose(source_code);

        return (!compiler_error) ? SUCCESS : compiler_error;
    }
