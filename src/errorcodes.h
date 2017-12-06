/*
    + ------------- +
    | PROJEKT IFJ17 |
    + ------------- +

    Společný projekt IFJ a IAL 2017
        Implementace prekladace imperativního jazyka IFJ17.

    Varianta zadanie:
        Tým 025, varianta I

    Soubor:
        errorcodes.h

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

    /**
     *  Error printout to stderr with details.
     */
    void raise_error(int error_code, const char *msg, ...);

#endif
