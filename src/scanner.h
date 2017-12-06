/*
    + ------------- +
    | PROJEKT IFJ17 |
    + ------------- +

    Společný projekt IFJ a IAL 2017
        Implementace prekladace imperativního jazyka IFJ17.

    Varianta zadanie:
        Tým 025, varianta I

    Soubor:
        scanner.h

    Autori:
        xmarko15 Peter Marko
        xmechl00 Stanislav Mechl
        xnanoa00 Andrej Nano
        xsvand06 Švanda Jan
*/

#ifndef SCNANNER_H
#define SCNANNER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>

#include "token.h"
#include "errorcodes.h"

/**
 * @brief Gets the next token form the source code
 *
 * Reads the file with getc(), ignores comments and creates tokens.
 * Modifies Token_t structure passed as parameter.
 * Reserves memory for char array with malloc().
 * Use free() on "token->value.c" if token_val_string
 * or token_identifier to not cause memory leaks.
 * When terminating the main program the scanner_free() must be used.
 * Returns boolean success.
 *
 * @param[in] File opened for reading
 * @param[in,out] Pointer to Token_t
 *
 */
bool get_next_token(FILE *f, Token_t *token);

/**
 * @brief Allocates token memory and initializes it
 *
 * Allocate token memory, initialize values and return token pointer.
 *
 * @param[out] Token_t* pointer to the new token or NULL
 */
Token_t *create_empty_token(void);

/**
 * @brief Initialize scanner
 *
 * Initializes scanners global static memory.
 *
 * @param[out] Boolean success
 */
bool scanner_init(void);

/**
 * @brief Free scanner memory
 *
 * Deallocates dynamic memory for scanner.
 */
void scanner_free(void);

#endif
