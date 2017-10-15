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

	Zadanie:
		Vytvorte program v jazyce C, který nacte zdrojový kód zapsaný ve zdrojovém
		jazyce IFJ17 a preloží jej do cílového jazyka IFJcode17 (mezikód).

*/

#include <stdio.h>

#include "errorcodes.h"
#include "token.h"
#include "scanner.h"

int main(int argc, char* argv[])
{

	// check arguments, input file @TODO make sepparate function for argument parsing
	if (argc != 2)
		return E_INTERNAL;

	FILE* source_code;  // make global ?

	if (! (source_code = fopen(argv[1], "r")))
		return E_INTERNAL;

	// structure of the program
		// option 1: call parse function, which inside calls scanner pointed to a global file variable.
		// option 2: call scanner with pointer to token vector as argument, call parser with updated token vector

	// scan(source_code);
	printf("Scanner test\n\n");

	// Token
	Token_t *token;

	// Read all tokens
	while (1) {

		// Get next token
		token = getNextToken(source_code);

		// parse();

		// Print token info
		printf("Token ID %d, value: %s (len: %u) @ line %u\n",
				token->type, token->value, token->size, token->line);

		// Check end
		if (token->type == token_end_of_file ||
			token->type == token_end_of_file_unexpected) {
			break; // Exit while loop
		}

		// Free allocated memory
		free(token->value);
		free(token);

	}

	// Free end of file token
	free(token->value);
	free(token);

	// Close file 
	fclose(source_code);

	return SUCCESS;

}
