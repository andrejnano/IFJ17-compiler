/*
    + ------------- +
    | PROJEKT IFJ17 |
    + ------------- +

    Společný projekt IFJ a IAL 2017
        Implementace prekladace imperativního jazyka IFJ17.

    Varianta zadanie:
        Tým 025, varianta I

    Soubor:
        scanner.c

    Autori:
        xmarko15 Peter Marko
        xmechl00 Stanislav Mechl
        xnanoa00 Andrej Nano
        xsvand06 Švanda Jan
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
} string_t;

// Initializes dynamic string
bool dStrInit(string_t *s)
{

    // Assert !NULL
    assert (s);

    // Allocates first bytes of memory
    s->str = (char *)malloc(sizeof(char) * DYNSTRING_INIT_LEN);

    // Check
    if (s->str == NULL)
    {
        raise_error(E_INTERNAL, "Memory error @dStrInit");

        return false;
    }

    // Initializes values
    s->str[0] = '\0';
    s->len = 0;
    s->max = DYNSTRING_INIT_LEN - 1;
    return true;
}

// Enlarge dynamic string
bool dStrEnlarge(string_t *s)
{

    // Assert !NULL
    assert(s);
    assert(s->str);

    // Reallocate more memory
    char *new_str = (char *)realloc(s->str, sizeof(char) * (s->max + DYNSTRING_RES_LEN + 1));

    // Check
    if (new_str == NULL)
    {
        raise_error(E_INTERNAL, "Memory error @dStrEnlarge");
        return false;
    }

    // Expand string
    s->max += DYNSTRING_RES_LEN;
    s->str = new_str;
    return true;
}

// Append character to the string
bool dStrAddChar(string_t *s, char c)
{

    // Assert !NULL
    assert(s);
    assert(s->str);

    // Check size
    if (s->len == s->max)
    {
        // Enlarge string
        if (!dStrEnlarge(s))
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
bool dStrOptimize(string_t *s)
{

    // Assert !NULL
    assert(s);
    assert(s->str);

    // Check size
    if (s->len != s->max)
    {

        // Reallocate memory
        char *new_str = (char *)realloc(s->str, sizeof(char) * (s->len + 1));

        // Check
        if (new_str == NULL)
        {
            raise_error(E_INTERNAL, "Memory error @dStrOptimize");
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

// Static global variables
static string_t value; // Token value
static unsigned int line; // Line counter
static bool end_of_file = true; // Identifies end of file
static bool empty_new_line = true; // Identify empty line

/**
 * @brief Initialize scanner
 *
 * Initializes scanners global static memory.
 *
 * @param[out] Boolean success
 */
bool scanner_init(void)
{

    // Reset line counter
    line = 1;

    // Set eof to false
    end_of_file = false;

    // Setup dynamic string
    if (dStrInit(&value))
        return true;
    return false;
}

/**
 * @brief Free scanner memory
 *
 * Deallocates dynamic memory for scanner.
 */
void scanner_free(void)
{

    // Free dynamic string
    line = 0;
    free(value.str);
    end_of_file = true;
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

    // Assert !NULL
    assert(s);

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

/**
 * @brief Allocates token memory and initializes it
 *
 * Allocate token memory, initialize values and return token pointer.
 *
 * @param[out] Token_t* pointer to the new token or NULL
 */
Token_t *create_empty_token(void)
{

    // Allocate token memory
    Token_t *token = (Token_t *)malloc(sizeof(Token_t));

    // Check token
    if (!token)
        return NULL;

    // Initialize token
    token->value.c = NULL;
    token->value.d = 0.0;
    token->value.i = 0;
    token->type = token_blank;
    token->line = 0;

    // Return token pointer
    return token;
}

// FSM states
typedef enum {
    EMPTY,
    ID,
    NUMBER,
    DECIMAL,
    DECIMAL_DOT,
    DOUBLE_EXP,
    DOUBLE_EXP_SIGN,
    DOUBLE,
    EXC_MARK,
    STRING,
    STRING_ESC,
    STRING_ESC_1N,
    STRING_ESC_2N,
    DIV_SIGN,
    COMMENT_SINGLE,
    COMMENT_MULTI,
    COMMENT_MULTI_END,
    OPER_LESS,
    OPER_GREAT
} fsm_state;

#define return_eof_false(ptr)    \
    {                            \
        (ptr)->type = token_eof; \
        return false;            \
    }

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
bool get_next_token(FILE *file, Token_t *token)
{

    // Expect file and token
    assert(file != NULL);
    assert(token != NULL);

    // Initialize FSM state
    fsm_state state = EMPTY;

    // Empty dynamic string
    (value.str)[0] = '\0';
    value.len = 0;

    // Reset token
    // if (token->value.c) free(token->value.c); // [?]
    token->value.c = NULL;
    token->value.d = 0.0;
    token->value.i = 0;

    // Cycle
    int strEcsValue = 0; // Escape character value
    int c = '\0'; // Character being read from the source

    // Reads source file
    while (!end_of_file)
    {

        // Next char
        c = getc(file);

        // Test for end
        if (c == EOF)
        {
            // Add final new line
            c = '\n';
            // Finish the output
            if (state == EMPTY || state == COMMENT_MULTI) end_of_file = true;
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

            // Update empty line check - line is not empty [\n] ['] [/]
            if (c!='\n' && c!=13 && c!='\'' && c!='/') empty_new_line = false;

            // Identifier beginning
            if ((c >= 'a' && c <= 'z') || c == '_')
            {
                if (!dStrAddChar(&value, c)) return_eof_false(token);
                state = ID;
            }
            // Number beginning
            else if (c >= '0' && c <= '9')
            {
                if (!dStrAddChar(&value, c)) return_eof_false(token);
                state = NUMBER;
            }
            // Single char switch
            else
            {
                switch (c)
                {
                case '/': state = DIV_SIGN; break; // Might be comment or div
                case '!': state = EXC_MARK; break; // String beginning
                case '\'': state = COMMENT_SINGLE; break; // Single line comm.
                case '<': state = OPER_LESS; break; // Might be <= or <> or <
                case '>': state = OPER_GREAT; break; // Might be >= or >
                case '*': token->type = token_op_mul; return true;
                case '\\': token->type = token_op_mod; return true;
                case '+': token->type = token_op_add; return true;
                case '-': token->type = token_op_sub; return true;
                case '=': token->type = token_op_eq; return true;
                case ',': token->type = token_comma; return true;
                case '(': token->type = token_lbrace; return true;
                case ')': token->type = token_rbrace; return true;
                case ';': token->type = token_semicolon; return true;
                case '\n': line++;
                    // Return only one EOL per group of EOL's
                    if (!empty_new_line)
                    {
                        empty_new_line = true;
                        token->type = token_eol;
                        return true;
                    }
                case 13: continue; // Windows CRLF compatibility
                default:
                    // Unknown character
                    raise_error(E_LEX, "Unknown character 0x%02x", c);
                    return_eof_false(token);
                }
            }
            break;
        case COMMENT_SINGLE:
            // Reads single line comment
            if (c == '\n')
            {
                // Comment end
                state = EMPTY;
                ungetc(c, file);
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
                empty_new_line = false;
                ungetc(c, file);
                token->type = token_op_div;
                return true;
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
            else
            {
                // Continues to be multi line comment
                state = COMMENT_MULTI;
                if (c == '\n') line++;
            }
            break;
        case ID:
            if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_')
            {
                // Identifier continues
                if (!dStrAddChar(&value, c)) return_eof_false(token);
            }
            else
            {

                // Return back last char
                ungetc(c, file);

                // Check if id is not keyword
                token_type keyword;
                if ((keyword = check_keyword(value.str)) != token_blank)
                {
                    // Returns keyword
                    token->type = keyword;
                    return true;
                }

                // Return identifier token
                token->type = token_identifier;

                // Fix identifier length
                if (!dStrOptimize(&value)) return_eof_false(token);

                // Pass string away
                token->value.c = value.str;

                // Get new string
                if (!dStrInit(&value)) {
                    // Take string away
                    value.str = token->value.c;
                    token->value.c = NULL;
                    return_eof_false(token);
                }

                // Return success
                return true;
            }
            break;
        case NUMBER:
            if (c >= '0' && c <= '9')
            {
                // Number continues
                if (!dStrAddChar(&value, c)) return_eof_false(token);
            }
            else if (c == '.')
            {
                // Becomes decimal number
                if (!dStrAddChar(&value, c)) return_eof_false(token);
                state = DECIMAL_DOT;
            }
            else if (c == 'e')
            {
                // Becomes double with exponent
                if (!dStrAddChar(&value, c)) return_eof_false(token);
                state = DOUBLE_EXP;
            }
            else
            {
                // Returns integer token
                ungetc(c, file);
                int val = (int) strtol(value.str, NULL, 10);
                token->type = token_val_integer;
                token->value.i = val;
                return true;
            }
            break;
        case DECIMAL:
            if (c >= '0' && c <= '9')
            {
                // Decimal continues
                if (!dStrAddChar(&value, c)) return_eof_false(token);
            }
            else if (c == 'e')
            {
                // Becomes double with exponent
                if (!dStrAddChar(&value, c)) return_eof_false(token);
                state = DOUBLE_EXP;
            }
            else
            {
                // Returns double token
                ungetc(c, file);
                double val = (double) strtod(value.str, NULL);
                token->type = token_val_double;
                token->value.d = val;
                return true;
            }
            break;
        case DECIMAL_DOT:
            if (c >= '0' && c <= '9')
            {
                // Decimal continues correctly
                if (!dStrAddChar(&value, c)) return_eof_false(token);
                state = DECIMAL;
            }
            else
            {
                // Bad double format (number.)
                ungetc(c, file);
                raise_error(E_LEX, "Wrong double format, got 0x%02x", c);
                return_eof_false(token);
            }
            break;            
        case DOUBLE_EXP:
            if (c >= '0' && c <= '9')
            {
                // Becomes full double
                if (!dStrAddChar(&value, c)) return_eof_false(token);
                state = DOUBLE;
            }
            else if (c == '+' || c == '-')
            {
                // Becomes double with exponent sign
                if (!dStrAddChar(&value, c)) return_eof_false(token);
                state = DOUBLE_EXP_SIGN;
            }
            else
            {
                // Bad double format (numE)
                ungetc(c, file);
                raise_error(E_LEX, "Wrong double format, got 0x%02x", c);
                return_eof_false(token);
            }
            break;
        case DOUBLE_EXP_SIGN:
            if (c >= '0' && c <= '9')
            {
                // Becomes full double
                if (!dStrAddChar(&value, c)) return_eof_false(token);
                state = DOUBLE;
            }
            else
            {
                // Bad double format (numberEsign)
                ungetc(c, file);
                raise_error(E_LEX, "Wrong double format, got 0x%02x", c);
                return_eof_false(token);
            }
            break;
        case DOUBLE:
            if (c >= '0' && c <= '9')
            {
                // Continues to be double
                if (!dStrAddChar(&value, c)) return_eof_false(token);
            }
            else
            {
                // Returns double token
                ungetc(c, file);
                double val = (double) strtod(value.str, NULL);
                token->type = token_val_double;
                token->value.d = val;
                return true;
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
                ungetc(c, file);
                raise_error(E_LEX, "Unknown character 0x%02x", c);
                return_eof_false(token);
            }
            break;
        case STRING:
            if (c == '\\')
            {
                // Reads escape sequence
                state = STRING_ESC;
                strEcsValue = 0;
            }
            else if (c == ' ')
            {
                // Space character
                if (!dStrAddChar(&value, '\\')) return_eof_false(token);
                if (!dStrAddChar(&value, '0')) return_eof_false(token);
                if (!dStrAddChar(&value, '3')) return_eof_false(token);
                if (!dStrAddChar(&value, '2')) return_eof_false(token);
            }
            else if (c == '\t')
            {
                // Tabulator character
                if (!dStrAddChar(&value, '\\')) return_eof_false(token);
                if (!dStrAddChar(&value, '0')) return_eof_false(token);
                if (!dStrAddChar(&value, '0')) return_eof_false(token);
                if (!dStrAddChar(&value, '9')) return_eof_false(token);
            }
            else if (c == '#')
            {
                // Hash sign character
                if (!dStrAddChar(&value, '\\')) return_eof_false(token);
                if (!dStrAddChar(&value, '0')) return_eof_false(token);
                if (!dStrAddChar(&value, '3')) return_eof_false(token);
                if (!dStrAddChar(&value, '5')) return_eof_false(token);
            }
            else if (c == '"')
            {

                // Finished reading string
                state = EMPTY;

                // Returns string token
                token->type = token_val_string;

                // Fix string length
                if (!dStrOptimize(&value)) return_eof_false(token);

                // Pass string away
                token->value.c = value.str;

                // Get new string
                if (!dStrInit(&value)) {
                    // Take string away
                    value.str = token->value.c;
                    token->value.c = NULL;
                    return_eof_false(token);
                }

                // Return success
                return true;

            }
            else if (c > 32)
            {
                // Regular printable character
                if (!dStrAddChar(&value, c)) return_eof_false(token);
            }
            else
            {
                // Wrong character string
                raise_error(E_LEX, "String contains bad character, 0x%02x is not allowed", c);
                return_eof_false(token);
            }
            break;
        case STRING_ESC:
            if (c >= '0' && c <= '2')
            {
                // Decimal escape sequence start
                strEcsValue = 100 * (c - '0');
                state = STRING_ESC_1N;
            }
            else if (c == 'n')
            {
                // Adds new line and continues as a string
                if (!dStrAddChar(&value, '\\')) return_eof_false(token);
                if (!dStrAddChar(&value, '0')) return_eof_false(token);
                if (!dStrAddChar(&value, '1')) return_eof_false(token);
                if (!dStrAddChar(&value, '0')) return_eof_false(token);
                state = STRING;
            }
            else if (c == 't')
            {
                // Adds tabulator and continues as a string
                if (!dStrAddChar(&value, '\\')) return_eof_false(token);
                if (!dStrAddChar(&value, '0')) return_eof_false(token);
                if (!dStrAddChar(&value, '0')) return_eof_false(token);
                if (!dStrAddChar(&value, '9')) return_eof_false(token);
                state = STRING;
            }
            else if (c == '\\') {
                // Adds backslash and continues as a string
                if (!dStrAddChar(&value, '\\')) return_eof_false(token);
                if (!dStrAddChar(&value, '0')) return_eof_false(token);
                if (!dStrAddChar(&value, '9')) return_eof_false(token);
                if (!dStrAddChar(&value, '2')) return_eof_false(token);
                state = STRING;
            }
            else if (c == '"')
            {
                // Adds quotation mark and continues as a string
                if (!dStrAddChar(&value, '"')) return_eof_false(token);
                state = STRING;
            }
            else
            {
                // Wrong escape sequence string
                raise_error(E_LEX, "Escape character with no meaning, got 0x%02x", c);
                return_eof_false(token);
            }
            break;
        case STRING_ESC_1N:
            if (c >= '0' && c <= '9')
            {
                // Decimal escape sequence continues
                strEcsValue += 10 * (c - '0');
                state = STRING_ESC_2N;
            }
            else
            {
                // Wrong escape sequence string
                raise_error(E_LEX, "Terminated escape sequence, expected number, got 0x%02x", c);
                return_eof_false(token);
            }
            break;
        case STRING_ESC_2N:
            if (c >= '0' && c <= '9')
            {
                // Decimal escape sequence ends
                strEcsValue += (c - '0');

                // Check invalidity of the character
                if (strEcsValue < 1 || strEcsValue > 255)
                {
                    raise_error(E_LEX, "Escape sequence out of bounds for %03d", strEcsValue);
                    return_eof_false(token);
                }

                // Check if escape sequence must be used
                if (strEcsValue < 33)
                {
                    // Add escape sequence
                    if (!dStrAddChar(&value, '\\')) return_eof_false(token);
                    if (!dStrAddChar(&value, '0')) return_eof_false(token);
                    if (!dStrAddChar(&value, '0'+strEcsValue/10)) return_eof_false(token);
                    if (!dStrAddChar(&value, '0'+strEcsValue%10)) return_eof_false(token);
                }
                else if (strEcsValue == 35)
                {
                    // Add hash sign
                    if (!dStrAddChar(&value, '\\')) return_eof_false(token);
                    if (!dStrAddChar(&value, '0')) return_eof_false(token);
                    if (!dStrAddChar(&value, '3')) return_eof_false(token);
                    if (!dStrAddChar(&value, '5')) return_eof_false(token);
                }
                else if (strEcsValue == 92)
                {
                    // Add backslash
                    if (!dStrAddChar(&value, '\\')) return_eof_false(token);
                    if (!dStrAddChar(&value, '0')) return_eof_false(token);
                    if (!dStrAddChar(&value, '9')) return_eof_false(token);
                    if (!dStrAddChar(&value, '2')) return_eof_false(token);
                }
                else
                {
                    // Add printable character
                    if (!dStrAddChar(&value, strEcsValue)) return_eof_false(token);
                }

                // Continue as string
                state = STRING;

                // Continue as string
                state = STRING;
            }
            else
            {
                // Wrong escape sequence string
                raise_error(E_LEX, "Terminated escape sequence, expected number, got 0x%02x", c);
                return_eof_false(token);
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
                ungetc(c, file);
                token->type = token_op_lt;
            }
            // Return token <?
            return true;
            break;
        case OPER_GREAT:
            if (c == '=')
            {
                token->type = token_op_ge;
            }
            else
            {
                ungetc(c, file);
                token->type = token_op_gt;
            }
            // Return token >?
            return true;
            break;
        } // FSM switch end
    }     // While getc() != EOL

    // Reached end of file
    token->type = token_eof;

    // Check state at the end
    if (state != EMPTY)
    {
        // Unexpected end of file
        raise_error(E_LEX, "End of file unexpected.");
        return false;
    }

    // Expected
    return true;

} // End get_next_token() definition
