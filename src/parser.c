#include "parser.h"

Token_t *active_token = NULL;
FILE *input = NULL, *output = NULL;
int counterVar = 1;
tSymbolTable *functions, *vars;

/*
 * \brief Initialisation of operand stack, used in expression evaluation
 */
void sInit(tStack **s)
{
	*s = NULL;
}

 /*
  * \brief Push new operand to top of the stack
  */
void sPush(tStack **s, tStack *val)
{
	tStack *top = (tStack *)malloc(sizeof(tStack));
	memcpy(top, val, sizeof(tStack));
	top->next = *s;
	*s = top;
}

 /*
  * \brief Pop operand from top of stack
  */
tStack *sPop(tStack **s)
{
	tStack *result = *s;
	*s = (*s)->next;
	return result;
}


 /*
  * \brief Generates new unused name
  */
void generateName(char **var)
{
	if (!output)
	{
		raise_error(1, "Error : not valid ouput file generateVariable\n");
		return;
	}
	static int count = 0;
	char *before = *var;
	*var = (char *)malloc(sizeof(*var) + 10);
	sprintf(*var,"$%s%u", before, count);
	fprintf(output, "defvar lf@%s\n", *var);
	count++;
}

 /*
  * \brief Generate code fore converting value
  * \param inType Type of source value
  * \param outType Type of destination value
  * \param val Source value to be converted
  * \param frame Mempry frame of source value, NULL if literal
  * \return name of new generated variable, in which is stored
  * converted value, NULL in case of error
  */
char *convert(int inType, int outType, token_value val, char *frame)
{
	char *tmpVar = "convert";
	switch (outType)
	{
	case  token_integer:
		if (inType != token_double)
		{
			raise_error(TYPE_ERROR, "Wrong type for conversion\n");
			return NULL;
		}
		generateName(&tmpVar);
		if (!frame)
			fprintf(output, "float2r2eint lf@%s float@%g\n", tmpVar, val.d);
		else
			fprintf(output, "float2r2eint lf@%s %s@%s\n", tmpVar, frame, val.c);
	break;
	case  token_double:
		if (inType != token_integer)
		{
			raise_error(TYPE_ERROR, "Wrong type for conversion\n");
			return NULL;
		}
		generateName(&tmpVar);
		if (!frame)
			fprintf(output, "int2float lf@%s int@%d\n", tmpVar, val.i);
		else
			fprintf(output, "int2float lf@%s %s@%s\n", tmpVar, frame, val.c);
	break;
	default:
		raise_error(TYPE_ERROR, "Wrong type for conversion\n");
		return NULL;
	}
	return tmpVar;
}

 /*
  * \brief Non terminal For reading func header and adding
  *  to symbol table list
  * \param dec True if function was already declared
  * \param funcname Here will be stored name of function
  */
void ntFunc(bool dec, char **funcName)
{
	
	match(active_token->type, token_function);
	node_val_t *val = STL_search(functions, active_token->value.c);
	if (active_token->type != token_identifier)
	{
		raise_error(SYNTAX_ERROR, "Expectd identifier\n");
		return;
	}
	*funcName = active_token->value.c;
	node_val_t funcVal;
	funcVal.dec = dec;
	funcVal.args = NULL;
	get_token_free();
	match(active_token->type, token_lbrace);
	tArglist **arglist = &(funcVal.args);
	// storing arguments to arglist
	while (1)
	{
		tArglist *node = funcVal.args;  // for testing if node with this name is already inserted
		while (node)
		{	
			// if name already there
			if (strcmp(node->name, active_token->value.c) == 0)
			{
				raise_error(SEM_ERROR, "Two arguments with same name\n");
				return;
			}
			node = node->next;
		}
		// new argument in list
		*arglist = (tArglist *)malloc(sizeof (tArglist));
		(*arglist)->name = active_token->value.c;
		(*arglist)->next = NULL;
		get_token_free();
		match(active_token->type, token_as);

		if (!istype(active_token->type))
		{
			raise_error( SYNTAX_ERROR, "Expectd type\n");
			return;
		}
		(*arglist)->type = active_token->type;
		arglist = &((*arglist)->next);
		get_token_free();
		if (active_token->type == token_comma)
		{
			get_token_free();
			continue;
		}
		else if (active_token->type != token_rbrace)
		{
			raise_error(SYNTAX_ERROR, "Expected token_rbrace");
			return;
		}
		else if (active_token->type == token_rbrace)
		{
			break;
		}
	}
	get_token_free();
	match(active_token->type, token_as);
	if (!istype(active_token->type))
	{
		raise_error(SYNTAX_ERROR, "Expected type\n");
		return;
	}
	funcVal.type = active_token->type; // storing type of function
	if ((val = STL_search(functions, *funcName)))
	{
		if (isSameArglists(val->args, funcVal.args) && 
			val->type == funcVal.type && funcVal.dec)
		{
			DisposeArgList(val->args);
		}
		else
		{
			raise_error( SEM_ERROR, "Redefinition of function %s\n");
			return;
		}
	}
	STL_insert_top(functions, *funcName, &funcVal);
	get_token_free();
	match(active_token->type, token_eol);
	if (!dec)
		fprintf(output, "label %s\n", *funcName);
}

/*
 * \brief Function for executing operation from top of stack
 * \param s Stack of operations
 * \param numOp Number of operators in stack
 * \return Executed operation, in case of error operation
 * with priority STACK_STOPPER
 */
tStack execOp (tStack **s, int *numOp)
{
	tStack tmp;
	if (*numOp < 2)
	{
		tmp.priority = STACK_STOPPER;
		tmp.type = STACK_STOPPER;
		return tmp;
	}
	tStack *operation = sPop(s);
	char *tmpName1 = "tmpName";
	char *tmpName2 = "tmpName";
	switch (operation->type)
	{
		case token_op_add:
			fprintf(output,"adds\n");
		break;
		case token_op_sub:
			fprintf(output,"subs\n");
		break;  
		case token_op_mul:
			fprintf(output,"muls\n");
		break;  
		case token_op_div:
			fprintf(output,"divs\n");
		break;  
		case token_op_mod:
			generateName(&tmpName1);
			generateName(&tmpName2);
			fprintf(output, "pops lf@%s\n", tmpName1);
			fprintf(output, "pops lf@%s\n", tmpName2);
			fprintf(output, "pushs lf@%s\n", tmpName2);
			fprintf(output, "pushs lf@%s\n", tmpName2);
			fprintf(output, "pushs lf@%s\n", tmpName1);
			fprintf(output, "divs\n");
			fprintf(output, "pushs lf@%s\n", tmpName1);
			fprintf(output, "muls\n");
			fprintf(output, "subs\n");
			free(tmpName1);
			free(tmpName2);
			fprintf(output,"divs\n");
		break;
		case token_op_eq:
			fprintf(output, "eqs\n");
		break;
		case token_op_lt:
			fprintf(output, "lts\npushs");
		break;
		case token_op_le:
			generateName(&tmpName1);
			generateName(&tmpName2);
			fprintf(output, "pops lf@%s\n", tmpName1);
			fprintf(output, "pops lf@%s\n", tmpName2);
			fprintf(output, "pushs lf@%s\n", tmpName2);
			fprintf(output, "pushs lf@%s\n", tmpName1);
			fprintf(output, "lts\n");
			fprintf(output, "pushs lf@%s\n", tmpName2);
			fprintf(output, "pushs lf@%s\n", tmpName1);
			fprintf(output, "eqs\nors\n");
		break;
		case token_op_ge:
			generateName(&tmpName1);
			generateName(&tmpName2);
			fprintf(output, "pops lf@%s\n", tmpName1);
			fprintf(output, "pops lf@%s\n", tmpName2);
			fprintf(output, "pushs lf@%s\n", tmpName2);
			fprintf(output, "pushs lf@%s\n", tmpName1);
			fprintf(output, "gts\n");
			fprintf(output, "pushs lf@%s\n", tmpName2);
			fprintf(output, "pushs lf@%s\n", tmpName1);
			fprintf(output, "eqs\nors\n");
		break;
		case token_op_ne:
			generateName(&tmpName1);
			generateName(&tmpName2);
			fprintf(output, "pops lf@%s\n", tmpName1);
			fprintf(output, "pops lf@%s\n", tmpName2);
			fprintf(output, "pushs lf@%s\n", tmpName2);
			fprintf(output, "pushs lf@%s\n", tmpName1);
			fprintf(output, "lts\n");
			fprintf(output, "pushs lf@%s\n", tmpName2);
			fprintf(output, "pushs lf@%s\n", tmpName1);
			fprintf(output, "gts\nors\n");
		break;
		case token_op_gt:
			fprintf(output, "gts\n");		
		break;
		case token_lbrace:
			free(sPop(s));
		break;
	}
	tmp = *operation;
	free(operation);
	(*numOp) -= 1;
	return tmp;
}

/*
 * \brief Function for converting token literal value to token
 * keword value, for example token_val_double to token_double;
 */
int TvalToKeyword(int val)
{
	switch (val)
	{
		case token_val_double:
			return token_double;
		case token_val_integer:
			return token_integer;
		case token_val_string:
			return token_string;
		// case token_val_boolean:
		// 	return token_boolean;
		default:
			return -1;
	}
	return 0;
}

/*
 * \brief Function for generating code for function call
 * \param vars Symbol table list containig metadata about variables
 * \param functions Symbol table list containig metadata about functions
 * \param funcname Name of function to be called
 */
void ntCallExpr(node_val_t *funcMeta, char *funcName, tSymbolTable *localVars)
{
	get_token_free();
	match(active_token->type, token_lbrace);
	fprintf(output, "createframe\n");
	tArglist *arg = funcMeta->args;
	while (active_token->type != token_rbrace)
	{

		fprintf(output, "defvar tf@%s\n", arg->name);
		if (active_token->type == token_identifier)
		{
			char *id = active_token->value.c;
			node_val_t *tmpMeta;
			if ((tmpMeta = STL_search(localVars, id)))
			{
				if (tmpMeta->dec)
				{
					raise_error(SEM_ERROR, "Use of uninitialised variable\n");
				}
				else if (tmpMeta->type == funcMeta->type)
				{
					fprintf(output, "move tf@%s lf@%s\n", arg->name, id);
				}
				else
				{
					char *name = convert(tmpMeta->type, arg->type, active_token->value, "lf");
					fprintf(output, "move tf@%s lf@%s\n", arg->name, name);
					free(name);
				}
				
			}
			else
			{
				raise_error(SEM_ERROR, "Use of undeclared identifier or use of function as argument\n");
				return;
			}
		}
		else if (TvalToKeyword(active_token->type) == arg->type)
		{
			fprintf(output, "move tf@%s ", arg->name);
			printTokenVal();
		}
		else
		{
			char *name = convert(TvalToKeyword(active_token->type), arg->type, active_token->value, NULL);
			fprintf(output, "move tf@%s lf@%s\n", arg->name, name);
			free(name);
		}
		arg = arg->next;
		get_token_free();
	}
	fprintf(output, "pushframe\ncall %s\n", funcName);
}

/*
 * \brief Function for generating code for expression evaluation
 * \param vars Symboltable list of variables
 * \param functions Symboltable list of functions
 * \param type Expected expression type
 */
void ntExpr(int type, tSymbolTable *localVars)
{
	tStack *s;
	sInit(&s);
	tStack insert; // pouzijem insert ako zarazku do stacku
	insert.priority = STACK_STOPPER;
	insert.type = STACK_STOPPER;
	sPush(&s, &insert);
	char *name = NULL;
	int numOp = 0;
	node_val_t *var;
	while (active_token->type != token_eol)
	{
		name = active_token->value.c;
		switch (active_token->type)
		{
		case token_op_add:
		case token_op_sub:
			insert.type = active_token->type;
			insert.priority = 3;
			if (s)
			while (s && s->priority <= insert.priority)
				execOp(&s, &numOp);
			sPush(&s, &insert);
			break;
		case token_op_div:
		case token_op_mul:
			insert.type = active_token->type;
			insert.priority = 1;
			if (s)
			while (s && s->priority <= insert.priority)
				execOp(&s, &numOp);
			sPush(&s, &insert);

			break;
		case token_op_mod:
			insert.type = active_token->type;
			insert.priority = 2;
			if (s && s->priority <= insert.priority)
				execOp(&s, &numOp);
			sPush(&s, &insert);
			break;
		case token_op_eq:
		case token_op_le:
		case token_op_lt:
		case token_op_ge:
		case token_op_ne:
		case token_op_gt:
			if (type != token_boolean)
			{
				raise_error(TYPE_ERROR, "Error at line %d : Expected boolean\n");
				return;
			}
			insert.type = active_token->type;
			insert.priority = 4;
			if (s && s->priority <= insert.priority)
			execOp(&s, &numOp);
			sPush(&s, &insert);
			break;
		case token_lbrace:
			insert.type = active_token->type;
			insert.priority = 6;
			sPush(&s, &insert);
			break;
		case token_rbrace:
			execOp(&s, &numOp);
			while (s->type != token_lbrace)
			{
				tStack tmp = execOp(&s, &numOp);
				if (tmp.priority >= STACK_STOPPER)
				{
					raise_error(TYPE_ERROR, "Not found corresponding left bracket\n");
					return;
				}
			}
			free(sPop(&s));
			break;
		case token_identifier:
			numOp++;
			if ((var = STL_search(localVars, active_token->value.c)))
			{
				if (var->type == token_string)
				{
					raise_error(TYPE_ERROR, "Wrong type\n");
					return;
				}
				else if (var->dec)
				{
					raise_error(SEM_ERROR, "Use of uninitialised variable\n");
				}
				else if (var->type != type)
				{
					name = convert(type, var->type, active_token->value, "lf");
					fprintf(output, "pushs lf@%s\n", name);
					free(name);
				}
				else
					fprintf(output, "pushs lf@%s\n", active_token->value.c);
			}
			else if ((var = STL_search(functions, active_token->value.c)))
			{
				ntCallExpr(var, active_token->value.c, localVars);
				if (type != var->type)
				{
					token_value tmpVal = active_token->value;
					tmpVal.c = "return";
					name = convert(var->type, type, tmpVal, "tf");
					fprintf(output, "pushs lf@%s\n", name);
					free(name);
				}
				else
				{
					fprintf(output, "pushs tf@return\n");
				}
				fprintf(output, "popframe\n");
			}

			break;
		case token_val_integer:
		case token_val_double:
			numOp++;
			if (TvalToKeyword(active_token->type) != type)
			{
				name = convert(TvalToKeyword(active_token->type), type, active_token->value, NULL);
				fprintf(output, "pushs lf@%s\n", name);
				free(name);
			}
			else
			{
				fprintf(output, "pushs ");
				printTokenVal();
			}
			break;
		default:
			raise_error(TYPE_ERROR, "Wrong type in expression\n");
			return;
		}
		get_token_free();
	}
	while (s->priority < STACK_STOPPER)
	{
		execOp(&s, &numOp);
	}
	free(sPop(&s));
}

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
