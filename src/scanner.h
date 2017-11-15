/**
 * @file Scanner.h
 * @author Jan Švanda
 * @date 2017-10-29
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

/**
 * @brief Gets the next token form the source code
 *
 * Reads the file with getc(), ignores comments and creates tokens.
 * Returns pointer to Token_t structure.
 * Reserves memory for token and char array with malloc().
 * Use free() on "token->value.c" if token_val_string
 * and "token" to not cause memory leaks.
 * Might return NULL if error occurred.
 * When terminating the main program other than after receiving NULL pointer
 * or the token_eof signalizing EOF the scanner_free() function must be used.
 *
 * @param[in] File opened for reading
 * @param[out] Pointer to Token_t
 *
 */
//Token_t *get_next_token(FILE *f);
void get_next_token(FILE *f, Token_t *t);

// Create empty token
// Return pointer to initialized empty token
Token_t *create_empty_token(void);

// Initialize and free scanner
/**
 * @brief Initialize scanner
 *
 * Must be used before using the get_next_token function.
 *
 * @param[out] Boolean success
 *
 */
bool scanner_init(void);

/**
 * @brief Free scanner
 *
 * Frees allocated private memory.
 * Use in case of terminating the main program.
 * No need to use this if get_next_token() returned NULL or EOF token.
 *
 */
void scanner_free(void);

#endif
