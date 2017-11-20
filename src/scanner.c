/**
 * @file Scanner.c
 * @author Jan Švanda
 * @date 2017-10-29
 * Implementation of source code scanner and lexical analysis.
 */

#include "scanner.h"

#define DYNSTRING_INIT_LEN 64
#define DYNSTRING_RES_LEN 256

// Dynamic string structure
typedef struct dynstring
{
    int len;
    int max;
    char *str;
}
string_t;

// Initializes dynamic string
bool strInit(string_t *s)
{

    // Assert !NULL
    assert (s != NULL);

    // Allocates first bytes of memory
    s->str = (char *) malloc(sizeof(char) * DYNSTRING_INIT_LEN);

    // Check
    if (s->str == NULL)
    {
        fprintf(stderr, "Memory error. Could not initialize string.");
        return false;
    }

    // Initializes values
    s->str[0] = '\0';
    s->len = 0;
    s->max = DYNSTRING_INIT_LEN - 1;
    return true;

}

// Empty dynamic string
void strEmpty(string_t *s)
{

    // Assert !NULL
    assert(s != NULL);
    assert(s->str != NULL);

    // Empty
    s->str[0] = '\0';
    s->len = 0;

}

// Enlarge dynamic string
bool strEnlarge(string_t *s)
{

    // Assert !NULL
    assert(s != NULL);
    assert(s->str != NULL);

    // Reallocate more memory
    char *new_str = (char *) realloc(s->str, sizeof(char) * (s->max + DYNSTRING_RES_LEN + 1));

    // Check
    if (new_str == NULL)
    {
        fprintf(stderr, "Memory error. Could not reallocate string.");
        return false;
    }

    // Expand string
    s->max += DYNSTRING_RES_LEN;
    s->str = new_str;
    return true;

}

// Append character to the string
bool strAddChar(string_t *s, char c)
{

    // Assert !NULL
    assert(s != NULL);
    assert(s->str != NULL);

    // Check size
    if (s->len == s->max)
    {
        // Enlarge string
        if (!strEnlarge(s))
        {
            return false;
        }
    }

    // Append character
    s->str[(s->len)++] = c;
    s->str[s->len] = '\0';
    return true;

}

// Optimize string size
bool strOptimize(string_t *s)
{

    // Assert !NULL
    assert(s != NULL);
    assert(s->str != NULL);

    // Check size
    if (s->len != s->max)
    {

        // Reallocate memory
        char *new_str = (char *) realloc(s->str, sizeof(char) * (s->len + 1));

        // Check
        if (new_str == NULL)
        {
            fprintf(stderr, "Memory error. Could not optimize string length.");
            return false;
        }

        // Set string values
        s->max = s->len;
        s->str = new_str;
        return true;

    }

    // Size max
    return true;

}

static string_t value; // Token value
static unsigned int line; // Line counter

// Initialize scanner
bool scanner_init(void)
{

    // Reset line counter
    line = 1;

    // Setup dynamic string
    if (strInit(&value))
        return true;
    return false;

}

// Free scanner - in case of terminating the main program
void scanner_free(void)
{

    // Free dynamic string
    line = 0;
    free(value.str);

}

/**
 * @brief Compares string to list of keywords
 *
 * Compares string to list of known keywords, returns keyword token_type
 * or if string not keyword returns token_blank value.
 *
 * @param[in] String to compare
 * @param[out] token_type
 */
token_type check_keyword(char *s)
{

    // Compare keywords ...

    if (strcmp("and", s) == 0)
    {
        return token_and;
    }
    else if (strcmp("as", s) == 0)
    {
        return token_as;
    }
    else if (strcmp("asc", s) == 0)
    {
        return token_asc;
    }
    else if (strcmp("boolean", s) == 0)
    {
        return token_boolean;
    }
    else if (strcmp("chr", s) == 0)
    {
        return token_chr;
    }
    else if (strcmp("continue", s) == 0)
    {
        return token_continue;
    }
    else if (strcmp("declare", s) == 0)
    {
        return token_declare;
    }
    else if (strcmp("dim", s) == 0)
    {
        return token_dim;
    }
    else if (strcmp("do", s) == 0)
    {
        return token_do;
    }
    else if (strcmp("double", s) == 0)
    {
        return token_double;
    }
    else if (strcmp("else", s) == 0)
    {
        return token_else;
    }
    else if (strcmp("elseif", s) == 0)
    {
        return token_elseif;
    }
    else if (strcmp("end", s) == 0)
    {
        return token_end;
    }
    else if (strcmp("exit", s) == 0)
    {
        return token_exit;
    }
    else if (strcmp("false", s) == 0)
    {
        return token_false;
    }
    else if (strcmp("for", s) == 0)
    {
        return token_for;
    }
    else if (strcmp("function", s) == 0)
    {
        return token_function;
    }
    else if (strcmp("if", s) == 0)
    {
        return token_if;
    }
    else if (strcmp("input", s) == 0)
    {
        return token_input;
    }
    else if (strcmp("integer", s) == 0)
    {
        return token_integer;
    }
    else if (strcmp("length", s) == 0)
    {
        return token_length;
    }
    else if (strcmp("loop", s) == 0)
    {
        return token_loop;
    }
    else if (strcmp("next", s) == 0)
    {
        return token_next;
    }
    else if (strcmp("not", s) == 0)
    {
        return token_not;
    }
    else if (strcmp("or", s) == 0)
    {
        return token_or;
    }
    else if (strcmp("print", s) == 0)
    {
        return token_print;
    }
    else if (strcmp("return", s) == 0)
    {
        return token_return;
    }
    else if (strcmp("scope", s) == 0)
    {
        return token_scope;
    }
    else if (strcmp("shared", s) == 0)
    {
        return token_shared;
    }
    else if (strcmp("static", s) == 0)
    {
        return token_static;
    }
    else if (strcmp("string", s) == 0)
    {
        return token_string;
    }
    else if (strcmp("substr", s) == 0)
    {
        return token_substr;
    }
    else if (strcmp("then", s) == 0)
    {
        return token_then;
    }
    else if (strcmp("true", s) == 0)
    {
        return token_true;
    }
    else if (strcmp("while", s) == 0)
    {
        return token_while;
    }
    else
    {
        return token_blank;
    }

}

// FSM states
typedef enum
{
    EMPTY, ID, NUMBER, DECIMAL, DOUBLE_EXP, DOUBLE_EXP_SIGN, DOUBLE,
    EXC_MARK, STRING, STRING_ESC, STRING_ESC_1N, STRING_ESC_2N,
    DIV_SIGN, COMMENT_SINGLE, COMMENT_MULTI, COMMENT_MULTI_END,
    OPER_LESS, OPER_GREAT
}
fsm_state;

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
Token_t *get_next_token(FILE *f)
{

    // Expect file
    assert(f != NULL);

    // Initialize FSM state and token memory
    fsm_state state = EMPTY;
    Token_t *token = (Token_t *) malloc(sizeof(Token_t));

    // Check token
    if (token == NULL)
    {
        fprintf(stderr, "Memory error. Could not allocate token.");
        free(value.str);
        return NULL;
    }

    // Initialize token
    token->type = token_blank;
    token->value.c = NULL;
    token->value.d = 0.0;
    token->value.i = 0;
    token->line = 0;

    // Cycle variables
    int strEcsValue; // Escape character value
    int c = '\0'; // Character being read from the source
    bool end = false; // Identifies end of file

    // Reads source file
    while (!end)
    {

        // Next char
        c = getc(f);

        // Test for end
        if (c == EOF)
        {
            // Add final new line
            c = '\n';
            end = true;
        }

        // Line count update
        token->line = line;

        // Blank space, new line, lowercase
        if (state == EMPTY && isblank(c))
        {
            // Skip blank space
            continue;
        }
        // If currently not in string
        else if (state != STRING)
        {
            // Make it lowercase
            c = tolower(c);
        }

        // FSM
        switch (state)
        {
        case EMPTY:
            // Identifier beginning
            if ((c>='a' && c<='z') || c == '_')
            {
                strAddChar(&value, c);
                state = ID;
            }
            // Number beginning
            else if (c>='0' && c<='9')
            {
                strAddChar(&value, c);
                state = NUMBER;
            }
            // Single line comment
            else if (c == '\'')
            {
                state = COMMENT_SINGLE;
            }
            // Might be operator
            else
            {
                // Operators switch
                switch (c)
                {
                case '/': state = DIV_SIGN; break; // Might be comment or div
                case '!': state = EXC_MARK; break; // Might be string or 'not'
                case '<': state = OPER_LESS; break; // Might be <= or <> or <
                case '>': state = OPER_GREAT; break; // Might be >= or >
                case '*':
                    token->type = token_op_mul;
                    return token;
                case '\\':
                    token->type = token_op_mod;
                    return token;
                case '+':
                    token->type = token_op_add;
                    return token;
                case '-':
                    token->type = token_op_sub;
                    return token;
                case '=':
                    token->type = token_op_eq;
                    return token;
                case ',':
                    token->type = token_comma;
                    return token;
                case '(':
                    token->type = token_lbrace;
                    return token;
                case ')':
                    token->type = token_rbrace;
                    return token;
                case ';':
                    token->type = token_semicolon;
                    return token;
                case '\n':
                    if (!end)
                    {
                        line++;
                        token->type = token_eol;
                        return token;
                    }
                    break;
                default:
                    // Unknown token type
                    token->type = token_blank;
                    return token;
                }
            }
            break;
        case COMMENT_SINGLE:
            // Reads single line comment
            if (c == '\n')
            {
                // Comment end
                state = EMPTY;
                ungetc(c, f);
            }
            break;
        case DIV_SIGN:
            if (c == '\'')
            {
                // Begins multi line comment
                state = COMMENT_MULTI;
            }
            else
            {
                // Not comment -> return division token
                ungetc(c, f);
                token->type = token_op_div;
                return token;
            }
            break;
        case COMMENT_MULTI:
            if (c == '\'')
            {
                // Might be the end of multi line comment
                state = COMMENT_MULTI_END;
            }
            else if (c == '\n')
            {
                // Counting source code lines
                line++;
            }
            break;
        case COMMENT_MULTI_END:
            if (c == '/')
            {
                // Multi line comment end
                state = EMPTY;
            }
            else if (c == '\'')
            {
                // Multi line might still end, keeps state
                continue;
            }
            else if (c == '\n')
            {
                // Counting source code lines
                line++;
            }
            else
            {
                // Continues to be multi line comment
                state = COMMENT_MULTI;
            }
            break;
        case ID:
            if ((c>='a' && c<='z') || (c>='0' && c<='9') || c == '_')
            {
                // Identifier continues
                strAddChar(&value, c);
            }
            else
            {

                // Return back last char
                ungetc(c, f);

                // Check if id is not keyword
                token_type keyword;
                if ((keyword = check_keyword(value.str)) != token_blank)
                {
                    // Returns keyword
                    strEmpty(&value);
                    token->type = keyword;
                    return token;
                }

                // Return identifier token
                token->type = token_identifier;
                // Fix inentifier length
                if (!strOptimize(&value))
                {
                    free(token);
                    return NULL;
                }
                // Pass string away
                token->value.c = value.str;

                // Get new string
                if (!strInit(&value))
                {
                    free(token->value.c);
                    free(value.str);
                    free(token);
                    return NULL;
                }

                // Return final token
                return token;

            }
            break;
        case NUMBER:
            if (c>='0' && c<='9')
            {
                // Number continues
                strAddChar(&value, c);
            }
            else if (c=='.')
            {
                // Becomes decimal number
                strAddChar(&value, c);
                state = DECIMAL;
            }
            else if (c=='e')
            {
                // Becomes double with exponent
                strAddChar(&value, c);
                state = DOUBLE_EXP;
            }
            else
            {
                // Returns integer token
                ungetc(c, f);
                int val = (int) strtol(value.str, NULL, 10);
                strEmpty(&value); // Empty string
                token->type = token_val_integer;
                token->value.i = val;
                return token;
            }
            break;
        case DECIMAL:
            if (c>='0' && c<='9')
            {
                // Decimal continues
                strAddChar(&value, c);
            }
            else if (c=='e')
            {
                // Becomes double with exponent
                strAddChar(&value, c);
                state = DOUBLE_EXP;
            }
            else
            {
                // Returns double token
                ungetc(c, f);
                double val = (double) strtod(value.str, NULL);
                strEmpty(&value); // Empty string
                token->type = token_val_double;
                token->value.d = val;
                return token;
            }
            break;
        case DOUBLE_EXP:
            if (c>='0' && c<='9')
            {
                // Becomes full double
                strAddChar(&value, c);
                state = DOUBLE;
            }
            else if (c=='+' || c=='-')
            {
                // Becomes double with exponent sign
                strAddChar(&value, c);
                state = DOUBLE_EXP_SIGN;
            }
            else
            {
                // Returns double token (broken double 123e) appends '0'
                ungetc(c, f);
                strAddChar(&value, '0');
                double val = (double) strtod(value.str, NULL);
                strEmpty(&value); // Empty string
                token->type = token_val_double;
                token->value.d = val;
                return token;
            }
            break;
        case DOUBLE_EXP_SIGN:
            if (c>='0' && c<='9')
            {
                // Becomes full double
                strAddChar(&value, c);
                state = DOUBLE;
            }
            else
            {
                // Returns double token (broken double 123e+) appends '0'
                ungetc(c, f);
                strAddChar(&value, '0');
                double val = (double) strtod(value.str, NULL);
                strEmpty(&value); // Empty string
                token->type = token_val_double;
                token->value.d = val;
                return token;
            }
            break;
        case DOUBLE:
            if (c>='0' && c<='9')
            {
                // Continues to be double
                strAddChar(&value, c);
            }
            else
            {
                // Returns double token
                ungetc(c, f);
                double val = (double) strtod(value.str, NULL);
                strEmpty(&value); // Empty string
                token->type = token_val_double;
                token->value.d = val;
                return token;
            }
            break;
        case EXC_MARK:
            if (c == '"')
            {
                // Becomes string
                state = STRING;
            }
            else
            {
                // Returns unknown token (!)
                ungetc(c, f);
                token->type = token_blank;
                return token;
            }
            break;
        case STRING:
            if (c == '\\')
            {
                // Reads escape sequence
                state = STRING_ESC;
                strEcsValue = 0;
            }
            else if (c == '"')
            {

                // Returns string token
                token->type = token_val_string;
                // Fix string length
                if (!strOptimize(&value))
                {
                    free(token);
                    return NULL;
                }
                // Pass string away
                token->value.c = value.str;

                // Get new string
                if (!strInit(&value)) {
                    free(token->value.c);
                    free(value.str);
                    free(token);
                    return NULL;
                }

                // Return final string token
                return token;

            }
            else if (c > 31)
            {
                // Continues as string
                strAddChar(&value, c);
                state = STRING;
            }
            else
            {
                // Wrong character string
                free(value.str);
                free(token);
                return NULL;
            }
            break;
        case STRING_ESC:
            if (c >= '0' && c <= '2')
            {
                // Decimal escape sequence start
                strEcsValue += 100*(c-'0');
                state = STRING_ESC_1N;
            }
            else if (c == 'n')
            {
                // Adds new line and continues as string
                strAddChar(&value, '\n');
                state = STRING;
            }
            else if (c == 't')
            {
                // Adds tabulator and continues as string
                strAddChar(&value, '\t');
                state = STRING;
            }
            else if (c == '"')
            {
                // Adds quotation mark and continues as string
                strAddChar(&value, '"');
                state = STRING;
            }
            else
            {
                // Wrong escape sequence string
                free(value.str);
                free(token);
                return NULL;
            }
            break;
        case STRING_ESC_1N:
            if (c >= '0' && c <= '9')
            {
                // Decimal escape sequence continues
                strEcsValue += 10*(c-'0');
                state = STRING_ESC_2N;
            }
            else
            {
                // Wrong escape sequence string
                free(value.str);
                free(token);
                return NULL;
            }
            break;
        case STRING_ESC_2N:
            if (c >= '0' && c <= '9')
            {
                // Decimal escape sequence ends
                strEcsValue += (c-'0');

                // Check invalidity of the character
                if (strEcsValue < 1 || strEcsValue > 255)
                {
                    free(value.str);
                    free(token);
                    return NULL;
                }

                // Add character, continue as string
                strAddChar(&value, strEcsValue);
                state = STRING;

            }
            else
            {
                // Wrong escape sequence string
                free(value.str);
                free(token);
                return NULL;
            }
            break;
        case OPER_LESS:
            switch (c)
            {
            case '>':
                token->type = token_op_ne;
                break;
            case '=':
                token->type = token_op_le;
                break;
            default:
                ungetc(c, f);
                token->type = token_op_lt;
            }
            // Return token <?
            return token;
            break;
        case OPER_GREAT:
            if (c == '=')
            {
                token->type = token_op_ge;
            }
            else
            {
                ungetc(c, f);
                token->type = token_op_gt;
            }
            // Return token >?
            return token;
            break;
        } // FSM switch end
    } // While getc() != EOL

    // Reached end of file
    if (state == COMMENT_MULTI || state == EMPTY)
    {
        // Correct end, return end of file
        token->type = token_eof;
        free(value.str);
        return token;
    }
    else
    {
        // Unexpected end of file
        free(value.str);
        free(token);
        return NULL;
    }

} // End get_next_token() definition
