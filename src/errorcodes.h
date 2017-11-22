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
    #include <stdio.h>
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
     * @brief Print formated error message - printf()
     *
     * Prints error message, creates custom prefix based on error_code,
     * appends '\n' character to the end of the message.
     */
    void raise_error(int error_code, const char *msg, ...);

#endif
