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

// temporary macro error codes as defined in ifj2017.pdf

#ifndef ERRORCODES_H
#define ERRORCODES_H

    #define ANSI_COLOR_RED     "\x1b[31m"
    #define ANSI_COLOR_GREEN   "\x1b[32m"
    #define ANSI_COLOR_YELLOW  "\x1b[33m"
    #define ANSI_COLOR_BLUE    "\x1b[34m"
    #define ANSI_COLOR_MAGENTA "\x1b[35m"
    #define ANSI_COLOR_CYAN    "\x1b[36m"
    #define ANSI_COLOR_RESET   "\x1b[0m"


    // everything OK
    #define SUCCESS         0

    //chyba v lexikální analýze (chybná struktura lexému).
    #define E_LEX           1

    //chyba v syntaktické analýze (chybná syntaxe programu).
    #define E_SYNTAX        2

    //semanticka chyba (nedef. funkcia/premenna, pokus o redefiniciu f./p.)
    #define E_SEM_DEF       3

    //semanticka chyba (typova kombatibilita v aritmetickych, retazovych a relacnych vyrazoch)
    #define E_SEM_TYPE      4

    //ostatne semanticke chyby
    #define E_SEM_OTHER     6

    //interna chyba prekladaca, neovplyvnena vstupnim programom. napr. bad mem alloc
    #define E_INTERNAL      99

    // global error flag - if error occurs,
    int compiler_error = 0;


    extern char *source_file_name;
    
    /**
     *  Error printout to stderr with details.
     */
    void raise_error(int error_code, const char *error_message)
    {
        int line = 2;

        switch (error_code)
        {
            case E_LEX:
                fprintf(stderr, "%s:%d:" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET "[%d][Lexical error] : %s\n",source_file_name, line, E_LEX, error_message); 
                break;
            case E_SYNTAX:
                fprintf(stderr, "%s:%d:" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET "[%d][Syntax error] : %s\n", source_file_name, line, E_SYNTAX, error_message);
                break;
            case E_SEM_DEF:
                fprintf(stderr, "%s:%d:" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET "[%d][Definiton error] : %s\n", source_file_name, line, E_SEM_DEF, error_message);
                break;
            case E_SEM_TYPE:
                fprintf(stderr, "%s:%d:" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET "[%d][Type error] : %s\n", source_file_name, line, E_SEM_TYPE, error_message);
                break;
            case E_SEM_OTHER:
                fprintf(stderr, "%s:%d:" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET "[%d][Semantic error] : %s\n", source_file_name, line, E_SEM_OTHER, error_message);
                break;
            case E_INTERNAL:
                fprintf(stderr, "%s:%d:" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET "[%d][Internal error] : %s\n", source_file_name, line, E_INTERNAL, error_message);
                break;
            default: 
                fprintf(stderr, "This error code --> ERROR[%d] is not defined by this program. \n It's message is : %s\n", error_code, error_message); 
                break;
        }

        // if the code is 0 (not changed yet)
        if (! compiler_error)
            compiler_error = error_code;
        
        // if error is already set, do not change it !!!! 
    }


#endif
