#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "errorcodes.h"

// global error flag - if error occurs,
int compiler_error = 0;
extern char *source_file_name;

/**
 *  Error printout to stderr with details.
 */
void raise_error(int error_code, const char *msg, ...)
{
    int line = 42;

    // VA list
    va_list args;
    va_start(args, msg);


    switch (error_code)
    {
    case E_LEX:
        fprintf(stderr, "%s:%d:" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET "[%d][Lexical error] : %s\n", source_file_name, line, E_LEX, msg);
        break;
    case E_SYNTAX:
        fprintf(stderr, "%s:%d:" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET "[%d][Syntax error] : %s\n", source_file_name, line, E_SYNTAX, msg);
        break;
    case E_SEM_DEF:
        fprintf(stderr, "%s:%d:" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET "[%d][Definiton error] : %s\n", source_file_name, line, E_SEM_DEF, msg);
        break;
    case E_SEM_TYPE:
        fprintf(stderr, "%s:%d:" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET "[%d][Type error] : %s\n", source_file_name, line, E_SEM_TYPE, msg);
        break;
    case E_SEM_OTHER:
        fprintf(stderr, "%s:%d:" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET "[%d][Semantic error] : %s\n", source_file_name, line, E_SEM_OTHER, msg);
        break;
    case E_INTERNAL:
        fprintf(stderr, "%s:%d:" ANSI_COLOR_RED "ERROR" ANSI_COLOR_RESET "[%d][Internal error] : %s\n", source_file_name, line, E_INTERNAL, msg);
        break;
    default:
        fprintf(stderr, "This error code --> ERROR[%d] is not defined by this program. \n It's message is : %s\n", error_code, msg);
        break;
    }

    // Print error message
    vfprintf(stderr, msg, args);
    va_end(args);

    // Append end of line
    fputs("\n", stderr);
    
    // if the code is 0 (not changed yet)
    if (!compiler_error)
    {
        // if error is already set, do not change it !!!!
        compiler_error = error_code;
    }
}