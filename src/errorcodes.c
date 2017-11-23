#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "errorcodes.h"
#include "token.h"

// global error flag - if error occurs,
int compiler_error = 0;
extern char *source_file_name;
extern Token_t *active_token;

/**
 *  Error printout to stderr with details.
 */
void raise_error(int error_code, const char *msg, ...)
{
    int line = active_token->line;

    // VA list
    va_list args;
    va_start(args, msg);

    fputs("\n", stderr);

    switch (error_code)
    {
    case E_LEX:
        fprintf(stderr, "%s:%d:" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET "[%d][Lexical error]: ", source_file_name, line, E_LEX);
        break;
    case E_SYNTAX:
        fprintf(stderr, "%s:%d:" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET "[%d][Syntax error]: ", source_file_name, line, E_SYNTAX);
        break;
    case E_SEM_DEF:
        fprintf(stderr, "%s:%d:" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET "[%d][Definiton error]: ", source_file_name, line, E_SEM_DEF);
        break;
    case E_SEM_TYPE:
        fprintf(stderr, "%s:%d:" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET "[%d][Type error]: ", source_file_name, line, E_SEM_TYPE);
        break;
    case E_SEM_OTHER:
        fprintf(stderr, "%s:%d:" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET "[%d][Semantic error]: ", source_file_name, line, E_SEM_OTHER);
        break;
    case E_INTERNAL:
        fprintf(stderr, "%s:%d:" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET "[%d][Internal error]: ", source_file_name, line, E_INTERNAL);
        break;
    default:
        fprintf(stderr, "This error code --> ERROR[%d] is not defined by this program. \n It's message is : \n\t", error_code);
        break;
    }
    // Print error message
    vfprintf(stderr, msg, args);
    va_end(args);
    fputs("\n", stderr);

    // if the code is 0 (not changed yet)
    if (!compiler_error)
    {
        // if error is already set, do not change it !!!!
        compiler_error = error_code;
    }
}
