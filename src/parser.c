#include "parser.h"
#include "expression.h"

Token_t *active_token = NULL;
FILE *input = NULL, *output = NULL;
int counterVar = 1;
tSymbolTable *functions, *vars;


/*
 * \brief Non terminal for evaluating body of function
 * \param localVars Symboltable list of variables for curr scope
 */
void ntCompoundStmt(tSymbolTable *localVars)
{
	char *id; // value of token identifier
	node_val_t newVal; // for inserting of new variable into symtable
	node_val_t *tmpMeta; // variable metadata for detecting of redefinition..

	switch (active_token->type)
	{
	case token_dim:
		get_token_free();
		if (STL_search(localVars, active_token->value.c))
		{
			raise_error(SEM_ERROR, "Redefinition of var %s\n");
			return;
		}
		if (active_token->type != token_identifier)
		{
			raise_error(SYNTAX_ERROR, "Expected identifier\n");
			return;
		}
		char *varName = active_token->value.c;
		get_token_free();
		if (active_token->type != token_as)
		{
			raise_error(SYNTAX_ERROR, "Expected 'as'\n");
			return;
		}
		get_token_free();
		if (!istype(active_token->type))
		{
			raise_error(SYNTAX_ERROR, "Expected type\n");
			return;
		}
		newVal.args = NULL;
		newVal.type = active_token->type;
		newVal.dec = true;
		fprintf(output, "defvar lf@%s\n", varName);
		zeroVarInit(varName);
		STL_insert_top(localVars, varName, &newVal);
		get_token_free();
	break;
	case token_identifier:
		// callfunc assign
		id = active_token->value.c;
		if ((tmpMeta = STL_search(localVars, id)))
		{
			get_token_free();
			if (active_token->type == token_op_eq) //assigment
			{
				get_token_free();
				ntExpr(tmpMeta->type, localVars);
				fprintf(output, "pops lf@%s\n", id);
				tmpMeta->dec = false;
			}
		}
		else
		{
			raise_error(SEM_ERROR, "Error at line %d : use of undeclared identifier %s\n"); 
			return;
		}
		break;
	case token_if:	// TODO
	printf("if\n");
		break;
	case token_while:
	printf("while\n");
		break;
	case token_for:
	printf("for\n");
		break;
	case token_eof:
		raise_error( SYNTAX_ERROR, "Error at line %d : unexpected EOF\n");
		return;
		break;
	default:
		break;
	}
	if (active_token->type == token_eol) {
		get_token_free();
		ntCompoundStmt(localVars);
	}
}

/*
 * \brief Non terminal for evaluating Head
 * \param vars Symboltable list of variables
 * \param functions Symboltable list of functions
 */
void ntHead()
{
	char *funcName = NULL;
	switch (active_token->type)
	{
	case  token_declare:
		get_token_free();
		ntFunc(1, &funcName);
		ntHead();
		break;
	case token_function:
		ntFunc(0, &funcName);
		// creating symboltable from function argList
		tSymbolTable *localVars = argsToSymtable(funcName, functions);
		ntCompoundStmt(localVars);
		STL_clean_up(&localVars);
		match(active_token->type, token_end);
		match(active_token->type, token_function);
		match(active_token->type, token_eol);
		ntHead();
		break;
	default:
		return;
	}
}

/*
 * \brief Main parsing function
 * \param in Source file in IFJ17 language
 * \param out Destination file in IFJcode17 language
 */
int parse(FILE *in, FILE *out)
{
	input = in;
	output = out;
	if ((active_token  = get_next_token(input)) == NULL)
		return LEX_ERROR; // nastala chyba jiz pri nacteni prvniho lexemu
	fprintf(output, ".IFJcode17\njump $main\n");
	STL_init(&functions);
	STL_init(&vars);

	ntHead();
	// ntScope(input, output, &functions);
	
	STL_clean_up(&functions);
	STL_clean_up(&vars);
	return 0;
}
