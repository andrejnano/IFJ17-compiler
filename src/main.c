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
    #include "scanner.h"
    #include "instructions.h"

    FILE* source_code;
    FILE* output_code;
    char* source_file_name;

    extern int compiler_error;

/******************************************************************************
    MAIN FUNCTION of the IFJ17 COMPILER
    - checks arguments
    - setups environment / source code file / source_code,output_code buffer/ symtable,...
    - start parser (syntax analyzer)
    - returns SUCESS or ERROR CODE
    - cleanup, close file, etc..

    @TODO how to output_code instruction code after compiling ???
 *****************************************************************************/

    int main(int argc, char* argv[])
    {
        // check arguments, source_code file @TODO make sepparate function for argument parsing
        // if (argc != 3)
        //     return E_INTERNAL;

        // if (! (source_code = fopen(argv[1], "r")))
        //     return E_INTERNAL;

        //output_code = stdout;
        // save the source name
        source_file_name = argv[1];
        

        // if (!(output_code = fopen(argv[2], "w")))
        //      return E_INTERNAL;

        source_code = stdin;
        output_code = stdout;

        parse();

        generate_code(output_code);

        //cleanup();
        fclose(source_code);
        fclose(output_code);

        // DEBUG
        #ifdef DEBUG
            if (compiler_error) printf(" EXIT STATUS : "ANSI_COLOR_RED"ERROR | %d \n\n" ANSI_COLOR_RESET, compiler_error);
            else                printf(" EXIT STATUS : " ANSI_COLOR_GREEN "SUCCESS | %d\n\n" ANSI_COLOR_RESET, compiler_error);
        #endif

        return SUCCESS;
    }
