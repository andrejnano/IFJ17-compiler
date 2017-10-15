/**
 * @file Scanner.h
 * @author Jan Švanda
 * @date 2017-10-15
 * Implementation of source code scanner and lexical analysis.
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

// TEMPORARY MEMORY ERROR
void memError(void);

/**
 * @brief Gets the next token form the source code
 *
 * Reads the file with getc(), ignores comments and creates value strings.
 * Returns pointer to Token_t structure with string value.
 * Reserves memory for token and char array with malloc().
 * Use free() on "token->value" and "token" to not cause memory leaks.
 *
 * @param[in] File opened for reading
 * @param[out] Pointer to Token_t
 */
Token_t *getNextToken(FILE *f);

#endif
