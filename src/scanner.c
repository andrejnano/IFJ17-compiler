/**
 * @file Scanner.c
 * @author Jan Švanda
 * @date 2017-10-15
 * Implementation of source code scanner and lexical analysis.
 */

#include "scanner.h"

// Temporary memory error call
void memError(void) { fprintf(stderr, "Memory error.\n"); exit(1); }

// Dynamic string structure
typedef struct dynstring {
	int len;
	int max;
	char *str;
} string_t;

// Initializes dynamic string
void strInit(string_t *s) {

	// Assert !NULL
	assert (s != NULL);

	// Allocates first 4 bytes
	s->str = malloc(sizeof(char) * 4);
	if (s->str == NULL) {
		memError();
		return;
	}

	// Initializes values
	s->str[0] = '\0';
	s->len = 0;
	s->max = 3;

}

// Empty dynamic string
void strEmpty(string_t *s) {

	// Assert !NULL
	assert(s != NULL);
	assert(s->str != NULL);

	// Empty
	s->str[0] = '\0';
	s->len = 0;

}

// Enlarge dynamic string
void strEnlarge(string_t *s) {

	// Assert !NULL
	assert(s != NULL);
	assert(s->str != NULL);

	// Allocate another 8 bytes
	char *new_str = malloc(sizeof(char) * s->max + 9);
	if (new_str == NULL) {
		memError();
		return;
	}

	// Expand string
	s->max += 8;
	strcpy(new_str, s->str);
	free(s->str);
	s->str = new_str;

}

// Append character to the string
void strAddChar(string_t *s, char c) {

	// Assert !NULL
	assert(s != NULL);
	assert(s->str != NULL);

	// Check size
	if (s->len == s->max) strEnlarge(s);

	// Append character
	s->str[(s->len)++] = c;
	s->str[s->len] = '\0';

}

// FSM states
typedef enum {EMPTY, ID, NUMBER, DECIMAL, DOUBLE_EXP, DOUBLE_EXP_SIGN, DOUBLE,
	EXC_MARK, STRING, STRING_ESC, STRING_ESC_1N, STRING_ESC_2N,
	DIV_SIGN, COMMENT_SINGLE, COMMENT_MULTI, COMMENT_MULTI_END,
	OPER_LESS, OPER_GREAT
} fsm_state;

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
Token_t *getNextToken(FILE *f) {

	// Expect file
	assert(f != NULL);

	// Initialize FSM state and token memory
	fsm_state state = EMPTY;
	Token_t *token = malloc(sizeof(Token_t));
	if (token == NULL) memError();
	token->type = EMPTY;
	token->value = NULL;
	token->line = 0;

	// Line counter
	static unsigned int line = 1;

	// Initialize token string memory
	string_t value;
	strInit(&value);

	// Cycle variables
	int strEcsValue;
	int c = '\0';
	bool end = false;

	// Reads source file
	while (!end) {

		// Test for end
		c = getc(f);
		if (c == EOF) {
			c = '\n';
			end = true;
		}

		// Skips blank space, makes lowercase
		if (state == EMPTY && (isblank(c) || c == '\n')) {
			// Count line and continue
			if (c == '\n') line++;
			continue;
		} else if (state != STRING) {
			// Make it lowercase (not in string)
			c = tolower(c);
		}

		// Line count update
		token->line = line;

		// FSM
		switch (state) {
		case EMPTY:
			if ((c>='a' && c<='z') || c == '_') {
				// Identifier beginning
				strAddChar(&value, c);
				state = ID;
			} else if (c>='0' && c<='9') {
				// Number beginning
				strAddChar(&value, c);
				state = NUMBER;
			} else if (c == '\'') {
				// Single line comment
				state = COMMENT_SINGLE;
			} else {
				// Operators
				strAddChar(&value, c);
				switch (c) {
				case '/': state = DIV_SIGN; break; // Might be comment
				case '!': state = EXC_MARK; break; // Might be string
				case '<': state = OPER_LESS; break; // Might be <= <>
				case '>': state = OPER_GREAT; break; // Might be >=
				case '*':
				case '\\':
				case '+':
				case '-':
				case '=':
					// Returns operator token
					token->type = token_operator;
					token->value = value.str;
					token->size = value.len;
					return token;
				default:
					// Unknown token type
					token->type = token_blank;
					token->value = value.str;
					token->size = value.len;
					token->size = value.len;
					return token;
				}
			}
			break;
		case COMMENT_SINGLE:
			// Read single line comment
			if (c == '\n') {
				// Comment end
				state = EMPTY;
				// Line counter
				line++;
			}
			break;
		case DIV_SIGN:
			if (c == '\'') {
				// Begins multi line comment
				strEmpty(&value);
				state = COMMENT_MULTI;
			} else {
				// Passes the division operator
				ungetc(c, f);
				token->type = token_operator;
				token->value = value.str;
				token->size = value.len;
				return token;
			}
			break;
		case COMMENT_MULTI:
			if (c == '\'') {
				// Might be the end of multi line comment
				state = COMMENT_MULTI_END;
			} else if (c == '\n') {
				// Counting source code lines
				line++;
			}
			break;
		case COMMENT_MULTI_END:
			if (c == '/') {
				// Multi line comment end
				state = EMPTY;
			} else if (c == '\'') {
				// Multi line might end, keeps state
			} else if (c == '\n') {
				// Counting source code lines
				line++;
			} else {
				// Continues to be multi line comment
				state = COMMENT_MULTI;
			}
			break;
		case ID:
			if ((c>='a' && c<='z') || (c>='0' && c<='9') || c == '_') {
				// Identifier continues
				strAddChar(&value, c);
			} else {
				// Returns identifier token
				// TODO? check if not keyword
				ungetc(c, f);
				token->type = token_identifier;
				token->value = value.str;
				token->size = value.len;
				return token;
			}
			break;
		case NUMBER:
			if (c>='0' && c<='9') {
				// Number continues
				strAddChar(&value, c);
			} else if (c=='.') {
				// Becomes decimal number
				strAddChar(&value, c);
				state = DECIMAL;
			} else if (c=='e') {
				// Becomes double with exponent
				strAddChar(&value, c);
				state = DOUBLE_EXP;
			} else {
				// Returns integer token
				ungetc(c, f);
				token->type = token_integer;
				token->value = value.str;
				token->size = value.len;
				return token;
			}
			break;
		case DECIMAL:
			if (c>='0' && c<='9') {
				// Decimal continues
				strAddChar(&value, c);
			} else if (c=='e') {
				// Becomes double with exponent
				strAddChar(&value, c);
				state = DOUBLE_EXP;
			} else {
				// Returns double token
				ungetc(c, f);
				token->type = token_double;
				token->value = value.str;
				token->size = value.len;
				return token;
			}
			break;
		case DOUBLE_EXP:
			if (c>='0' && c<='9') {
				// Becomes full double
				strAddChar(&value, c);
				state = DOUBLE;
			} else if (c=='+' || c=='-') {
				// Becomes double with exponent sign
				strAddChar(&value, c);
				state = DOUBLE_EXP_SIGN;
			} else {
				// Returns double token (broken double 123e) appends '0'
				ungetc(c, f);
				strAddChar(&value, '0');
				token->type = token_double;
				token->value = value.str;
				token->size = value.len;
				return token;
			}
			break;
		case DOUBLE_EXP_SIGN:
			if (c>='0' && c<='9') {
				// Becomes full double
				strAddChar(&value, c);
				state = DOUBLE;
			} else {
				// Returns double token (broken double 123e+) appends '0'
				ungetc(c, f);
				strAddChar(&value, '0');
				token->type = token_double;
				token->value = value.str;
				token->size = value.len;
				return token;
			}
			break;
		case DOUBLE:
			if (c>='0' && c<='9') {
				// Continues to be double
				strAddChar(&value, c);
			} else {
				// Returns double token
				ungetc(c, f);
				token->type = token_double;
				token->value = value.str;
				token->size = value.len;
				return token;
			}
			break;
		case EXC_MARK:
			if (c == '"') {
				// Becomes string
				strEmpty(&value);
				state = STRING;
			} else {
				// Returns unknown token (!)
				ungetc(c, f);
				token->type = token_blank;
				token->value = value.str;
				token->size = value.len;
				return token;
			}
			break;
		case STRING:
			if (c == '\\') {
				// Reads escape sequence
				state = STRING_ESC;
				strEcsValue = 0;
			} else if (c == '"') {
				// Returns string token
				token->type = token_string;
				token->value = value.str;
				token->size = value.len;
				return token;
			} else if (c > 31) {
				// Continues as string
				strAddChar(&value, c);
				state = STRING;
			} else {
				// Returns wrong character string
				ungetc(c, f);
				token->type = token_string_wrong_character;
				token->value = value.str;
				token->size = value.len;
				return token;
			}
			break;
		case STRING_ESC:
			if (c >= '0' && c <= '2') {
				// Decimal escape sequence start
				strEcsValue += 100*(c-'0');
				state = STRING_ESC_1N;
			} else if (c == 'n') {
				// Adds new line and continues as string
				strAddChar(&value, '\n');
				state = STRING;
			} else if (c == 't') {
				// Adds tabulator and continues as string
				strAddChar(&value, '\t');
				state = STRING;
			} else if (c == '"') {
				// Adds quotation mark and continues as string
				strAddChar(&value, '"');
				state = STRING;
			} else {
				// Returns wrong escape sequence string
				ungetc(c, f);
				token->type = token_string_wrong_esc_seq;
				token->value = value.str;
				token->size = value.len;
				return token;
			}
			break;
		case STRING_ESC_1N:
			if (c >= '0' && c <= '9') {
				// Decimal escape sequence continues
				strEcsValue += 10*(c-'0');
				state = STRING_ESC_2N;
			} else {
				// Returns wrong escape sequence string
				ungetc(c, f);
				token->type = token_string_wrong_esc_seq;
				token->value = value.str;
				token->size = value.len;
				return token;
			}
			break;
		case STRING_ESC_2N:
			if (c >= '0' && c <= '9') {
				// Decimal escape sequence ends
				strEcsValue += (c-'0');
				// Check invalidity of the character
				if (strEcsValue < 1 || strEcsValue > 255) {
					token->type = token_string_wrong_esc_seq;
					token->value = value.str;
					token->size = value.len;
					return token;
				}
				// Add character, continue as string
				strAddChar(&value, strEcsValue);
				state = STRING;
			} else {
				// Returns wrong escape sequence string
				ungetc(c, f);
				token->type = token_string_wrong_esc_seq;
				token->value = value.str;
				token->size = value.len;
				return token;
			}
			break;
		case OPER_LESS:
			if (c == '>' || c == '=') {
				// Return <> / <= token
				strAddChar(&value, c);
				token->type = token_operator;
				token->value = value.str;
				token->size = value.len;
				return token;
			} else {
				// Returns single operator token
				ungetc(c, f);
				token->type = token_operator;
				token->value = value.str;
				token->size = value.len;
				return token;
			}
			break;
		case OPER_GREAT:
			if (c == '=') {
				// Return >= token
				strAddChar(&value, c);
				token->type = token_operator;
				token->value = value.str;
				token->size = value.len;
				return token;
			} else {
				// Returns single operator token
				ungetc(c, f);
				token->type = token_operator;
				token->value = value.str;
				token->size = value.len;
				return token;
			}
			break;
		} // FSM switch end
	} // While getc() != NULL

	// Reached end of file
	if (state == COMMENT_MULTI || state == EMPTY) {
		// Correct end, free string pointer return end of file
		token->type = token_end_of_file;
		token->value = value.str;
		token->size = value.len;
		return token;
	} else {
		// Unexpected end of file
		token->type = token_end_of_file_unexpected;
		token->value = value.str;
		token->size = value.len;
		return token;
	}

}
