#include "parser.h"
Token_t *cur_token;
FILE *input, *output;
int counterVar = 1;

int generateVariable(char *var)
{
	if (!output) {
		fprintf(stderr, "Error : not valid ouput file generateVariable\n");
		return 1;
	}
	static int count = 0;
	fprintf(output,"%s%u", var, count);
	count++;
	return 0;
}

int generateLabel(char *var)
{
	if (!output) {
		fprintf(stderr, "Error : not valid ouput file generateLabel\n");
		return 1;
	}
	static int count = 0;
	fprintf(output,"%s%u", var, count);
	count++;
	return 0;
}
void get_token_free() {
	if (cur_token) {
		free(cur_token);
	}
	cur_token = get_next_token(input);
}

int ntFunc(tSymbolTable *functions, bool dec, char **funcName) {
	
	if (cur_token->type != token_function) {
		fprintf(stderr, "Error at line %d: expected function\n", cur_token->line);
		return SYNTAX_ERROR;
	}
	get_token_free();
	node_val_t *val = ST_search(functions, cur_token->value.c);
	if (!cur_token->value.c) {
		fprintf(stderr, "Error at line %d: expectd identifier\n", cur_token->line);
		return SYNTAX_ERROR;
	} else if ((val = ST_search(functions, cur_token->value.c))
		 && val->dec == dec) {
		fprintf(stderr, "Error at line %d: redefinition of function %s\n", cur_token->line, cur_token->value.c);
		return SEM_ERROR;
	}	
	*funcName = cur_token->value.c;
	node_val_t *funcVal = (node_val_t *)malloc(sizeof(node_val_t));
	memset(funcVal, 0, sizeof(node_val_t));
	funcVal->dec = dec;
	get_token_free();
	if (cur_token->type != token_lbrace) {
		fprintf(stderr, "Error at line %d: expectd left bracket\n", cur_token->line);
		return SYNTAX_ERROR;
	}
	get_token_free();
	tArglist **arglist = &(funcVal->args);
	while (cur_token->type != token_rbrace) {

		*arglist = (tArglist *)malloc(sizeof (tArglist));
		(*arglist)->name = cur_token->value.c;
		(*arglist)->next = NULL;
		get_token_free();
		if (cur_token->type != token_as) {
			fprintf(stderr, "Error at line %d: expectd 'as'\n", cur_token->line);
			return SYNTAX_ERROR;
		}
		get_token_free();
		
		switch (cur_token->type)
		{
		case  token_integer:
			(*arglist)->type = TYPE_INT;
			break;
		case  token_double:
			(*arglist)->type = TYPE_DOUBLE;
			break;
		case  token_string:
			(*arglist)->type = TYPE_STRING;
			break;
		case  token_boolean:
			(*arglist)->type = TYPE_BOOLEAN;
			break;
		default:
			fprintf(stderr, "Error at line %d: expectd type\n", cur_token->line);
			return SYNTAX_ERROR;
		}
		arglist = &((*arglist)->next);
		get_token_free();
		if (cur_token->type != token_comma)
			break;
		get_token_free();
	}
	get_token_free();
	if (cur_token->type != token_as) {
		fprintf(stderr, "Error at line %d: expectd 'as'\n", cur_token->line);
		return SYNTAX_ERROR;
	}
	get_token_free();
	switch (cur_token->type)
	{
	case  token_integer:
		funcVal->type = TYPE_FUNCTION_INT;
		break;
	case  token_double:
		funcVal->type = TYPE_FUNCTION_DOUBLE;
		break;
	case  token_string:
		funcVal->type = TYPE_FUNCTION_STRING;
		break;
	case  token_boolean:
		funcVal->type = TYPE_FUNCTION_BOOL;
		break;
	default:
		fprintf(stderr, "Error at line %d: expectd type\n", cur_token->line);
		return SYNTAX_ERROR;
	}
	ST_insert(functions, *funcName, funcVal);
	get_token_free();
	if (cur_token->type != token_eol && cur_token->type != token_eof) {
		fprintf(stderr, "Error at line %d: expectd EOL\n", cur_token->line);
		return SYNTAX_ERROR;
	}
	if (!dec)
		fprintf(output, "label %s\n", *funcName);
	get_token_free();
	return 0;
}

int ntCompoundStmt(tSymbolTable *functions, char *funcName) {
	fprintf(output, "createframe\n");
	tSymbolTable args;
	ST_init(&args);
	tSymbolTable local_vars;
	ST_init(&local_vars);
	tArglist *argList = ST_search(functions, funcName)->args;
	while (argList) {
		node_val_t val;
		val.args = NULL;
		val.type = argList->type;
		ST_insert(&args, argList->name, &val);
		argList = argList->next;
	}
	while (cur_token->type != token_end) {
		switch (cur_token->type)
		{
		case token_dim:
			get_token_free();
			if (cur_token->type != token_identifier) {
				fprintf(stderr, "Error at line %d: expected identifier\n", 	cur_token->line);
				return SYNTAX_ERROR;
			}
			char *varName = cur_token->value.c;
			get_token_free();
			if (cur_token->type != token_as) {
				fprintf(stderr, "Error at line %d: expected 'as'\n", 	cur_token->line);
				return SYNTAX_ERROR;
			}
			get_token_free();
			node_val_t varVal;
			varVal.args = NULL;
			switch (cur_token->type)
			{
			case  token_integer:
				varVal.type = TYPE_INT;
				break;
			case  token_double:
				varVal.type = TYPE_DOUBLE;
				break;
			case  token_string:
				varVal.type = TYPE_STRING;
				break;
			case  token_boolean:
				varVal.type = TYPE_BOOLEAN;
				break;
			default:
				fprintf(stderr, "Error at line %d: expected type\n", 	cur_token->line);
				return SYNTAX_ERROR;
			}
			fprintf(output, "defvar tf@%s\n", varName);
			ST_insert(&local_vars, varName, &varVal);
			get_token_free();
			if (cur_token->type == token_op_eq) {
				get_token_free();
				// ntExp(input, output); // todo
			}
			break;
			char *id;
			case token_identifier:
				// callfunc assign
				id = cur_token->value.c;
				if (ST_search(functions, id)) { //call stmt
					// todo porovnat vsetky typy premennych
				} else if (ST_search(&local_vars, id)) {
					get_token_free();
					if (cur_token->type == token_op_eq) //assigment
					{
					}
				} else if (ST_search(&args, id)) { //probably assigment

				}
				else {
					fprintf(stderr, "Error at line %d : use of undeclared identifier %s\n", cur_token->line, id);
					return SEM_ERROR;
				}
				break;
			case token_if:
			printf("if\n");
				break;
			case token_while:
			printf("while\n");
				break;
			case token_for:
			printf("for\n");
				break;
			case token_eof:
				fprintf(stderr, "Error at line %d : unexpected EOF\n", cur_token->line);
				return SYNTAX_ERROR;
				break;
			default:
				break;
		}
		get_token_free();
	}
	get_token_free();
	return 0;
}
int ntHead(tSymbolTable *functions) {
	int tmp;
	char *funcName;
	switch (cur_token->type)
	{
	case  token_declare:
		get_token_free();
		tmp = ntFunc(functions, 1, &funcName);
		if (tmp)
			return tmp;
		tmp = ntHead(functions);
		if (tmp)
			return tmp;
		break;
	case token_function:
		tmp = ntFunc(functions, 0, &funcName);
		if (tmp)
			return tmp;
		tmp = ntCompoundStmt(functions, funcName);
		tmp = ntHead(functions);
		if (tmp)
			return tmp;
		break;
	default:
		return 0;
	}
	return 0;
}


int parse(FILE *in, FILE *out)
{
	input = in;
	output = out;
	if ((cur_token  = get_next_token(input)) == NULL)
		return LEX_ERROR; // nastala chyba jiz pri nacteni prvniho lexemu
	fprintf(output, ".IFJcode17\njump $main\n");
	tSymbolTable functions;
	ST_init(&functions);

	ntHead(&functions);
	// ntScope(input, output, &functions);
	get_token_free();
	if (cur_token->type != token_eof) {
		fprintf(stderr, "Error : expected EOF");
		free(cur_token);
		return SYNTAX_ERROR;
	}
	free(cur_token);
	return 0;
}
