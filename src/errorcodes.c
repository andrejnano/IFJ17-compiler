#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "errorcodes.h"

// global error flag - if error occurs,
static int compiler_error = 0;

/**
 *  Error printout to stderr with details.
 */
void raise_error(int error_code, const char *msg, ...)
{

    // VA list
    va_list args;
    va_start(args, msg);

    // Print error type
    switch (error_code)
    {
        case E_LEX:         fprintf(stderr, "ERROR[%d][Lexical error] : ", E_LEX); break;
        case E_SYNTAX:      fprintf(stderr, "ERROR[%d][Syntax error] : ", E_SYNTAX); break;
        case E_SEM_DEF:     fprintf(stderr, "ERROR[%d][Definiton error] : ", E_SEM_DEF); break;
        case E_SEM_TYPE:    fprintf(stderr, "ERROR[%d][Type error] : ", E_SEM_TYPE); break;
        case E_SEM_OTHER:   fprintf(stderr, "ERROR[%d][Semantic error] : ", E_SEM_OTHER); break;
        case E_INTERNAL:    fprintf(stderr, "ERROR[%d][Internal error] : ", E_INTERNAL); break;
        default: fprintf(stderr, "Undefined ERROR[%d] : ", error_code); break;
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
