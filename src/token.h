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
*/

// TOKEN DATA STRUCTURE

#ifndef TOKEN_H
#define TOKEN_H

	// vnutorny konkretny typ tokenu
	typedef enum
	{
		token_blank,
		token_identifier,
		token_integer,
		token_double,
		token_string,
		token_string_missing_term,
		token_string_wrong_esc_seq,
		token_string_wrong_character,
		token_end_of_file,
		token_end_of_file_unexpected,
		token_operator,
		token_variable,
		token_semicolon,
		// ... bude ich asi urcite viac, neviem ale ake vsetky chceme,
	} token_type;


	// datovy typ pre objekt 'token'
	typedef struct _TOKEN
	{
		token_type type;
		unsigned int size;
		unsigned int line;
		char* value;
	} Token_t;

	/*
		Token_t mytoken;
		mytoken.type = token_identifier;
		mytoken.value = "count";
		mytoken.size = strlen(mytoken.value); ? ? ?
	*/
#endif
