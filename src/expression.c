#include "expression.h"
#include "symtable.h"
#include "errors.h"
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
 * \brief Initialization of variable to default starting value
 * \param varName Name of variable to be initialized
 */
void zeroVarInit(char *varName) {
	switch (active_token->type)
	{
		case token_integer:
			fprintf(output, "move lf@%s int@0\n", varName);
			break;
		case token_double:
			fprintf(output, "move lf@%s float@0.0\n", varName);
			break;
		case token_string:
			fprintf(output, "move lf@%s string@\n", varName);
			break;
		case token_boolean:
			fprintf(output, "move lf@%s bool@false\n", varName);
			break;
		default :
			raise_error(SYNTAX_ERROR, "Expecet type int, double ...\n");
	}
}

 /*
  * \brief Non terminal For reading func header and adding
  *  to symbol table list
  * \param If function is being declared
  * \param funcname Here will be stored name of function
  */
void ntFunc(bool dec, char **funcName)
{
	match(active_token->type, token_function);
	// saving metadata about function which could be already declared for
	// later comparison
	node_val_t *oldFuncMeta = STL_search(functions, active_token->value.c);
	if (active_token->type != token_identifier)
	{
		raise_error(SYNTAX_ERROR, "Expectd identifier\n");
		return;
	}
	*funcName = active_token->value.c;
	// creating variable for later storing and comparing with oldFuncMeta
	node_val_t newFuncMeta;
	newFuncMeta.dec = dec;
	newFuncMeta.args = NULL;
	get_token_free();
	match(active_token->type, token_lbrace);
	// storing arguments to arglist
	tArglist *argList = NULL;
	// storing arguments to newFuncMeta.args
	while (active_token->type != token_rbrace)
	{	
		tArglist *tmpNode = (tArglist *)malloc(sizeof (tArglist));
		tmpNode->name = active_token->value.c;
		tmpNode->next = NULL;
		get_token_free();
		match(active_token->type, token_as);

		if (!istype(active_token->type))
		{
			raise_error( SYNTAX_ERROR, "Expectd type\n");
			return;
		}
		tmpNode->type = active_token->type;
		get_token_free();
		// token comma means there will be other argument so continue
		if (active_token->type == token_comma)
		{
			get_token_free();
			if (argListAppend(&argList, tmpNode))
				return;
			continue;
		}
		else if (active_token->type != token_rbrace)
		{
			raise_error(SYNTAX_ERROR, "Expected token_rbrace\n");
			return;
		}
		if (argListAppend(&argList, tmpNode))
			return;
	}

	get_token_free();
	match(active_token->type, token_as);
	if (!istype(active_token->type))
	{
		raise_error(SYNTAX_ERROR, "Expected type\n");
		return;
	}
	newFuncMeta.type = active_token->type;
	newFuncMeta.args = argList;
	if (oldFuncMeta && (oldFuncMeta->dec == dec || (!oldFuncMeta->dec && dec) || 
		!isSameArglists(newFuncMeta.args, oldFuncMeta->args) || newFuncMeta.type != oldFuncMeta->type))
	{
		raise_error( SEM_ERROR, "Redefinition of function %s\n");
		DisposeArgList(argList);
		return;
	}
	// if there were no old metadata insert new
	// otherwise no need for inserting new ones consistence were checked

	if (!oldFuncMeta)
		STL_insert_top(functions, *funcName, &newFuncMeta);
	else
		DisposeArgList(newFuncMeta.args);
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
	tmp.priority = STACK_STOPPER;
	if (*numOp < 2)
	{
		raise_error(SYNTAX_ERROR, "Wrong syntax of expression\n");
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
	// read check types and convert arguments for calling function
	while (active_token->type != token_rbrace)
	{
		if (!arg)	// no bracket but still expected argument
		{
			raise_error(SEM_ERROR, "Too many arguments for function\n");
			return;
		}
		fprintf(output, "defvar tf@%s\n", arg->name);
		if (active_token->type == token_identifier)
		{
			char *id = active_token->value.c;
			node_val_t *tmpMeta;
			if ((tmpMeta = STL_search(localVars, id)))
			{
				if (tmpMeta->type == funcMeta->type)
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
		// tvalToKeyword returns corresponding keyword to literal for example token_val_integer -> token_integer
		// it is used because types are in structures stored as keyword for example token_integer
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
		if (active_token->type != token_comma)
			break;
		get_token_free();
	}

	if (arg)
	{
		raise_error(SEM_ERROR, "Too few arguments for function\n");
		return;
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
			while (s && s->priority && s->priority <= insert.priority)
				if (execOp(&s, &numOp).priority == STACK_STOPPER)
					return;
			sPush(&s, &insert);
			break;
		case token_op_div:
		case token_op_mul:
			insert.type = active_token->type;
			insert.priority = 1;
			if (s)
			while (s && s->priority && s->priority <= insert.priority)
				if (execOp(&s, &numOp).priority == STACK_STOPPER)
					return;
			sPush(&s, &insert);

			break;
		case token_op_mod:
			insert.type = active_token->type;
			insert.priority = 2;
			while (s && s->priority && s->priority <= insert.priority)
				if (execOp(&s, &numOp).priority == STACK_STOPPER)
					return;
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
			while (s && s->priority && s->priority <= insert.priority)
				if (execOp(&s, &numOp).priority == STACK_STOPPER)
					return;
			sPush(&s, &insert);
			break;
		case token_lbrace:
			insert.type = active_token->type;
			insert.priority = 6;
			sPush(&s, &insert);
			break;
		case token_rbrace:
			if (execOp(&s, &numOp).priority == STACK_STOPPER)
				return;
			while (s->type != token_lbrace)
			{
				if (execOp(&s, &numOp).priority == STACK_STOPPER)
					return;
			}
			free(sPop(&s));
			break;
		case token_identifier:
			numOp++;
			if ((var = STL_search(localVars, active_token->value.c)))
			{
				// if (type != var->type && type != token_boolean)
				// {
				// 	name = convert(type, var->type, active_token->value, "lf");
				// 	fprintf(output, "pushs lf@%s\n", name);
				// 	free(name);
				// }
				// else
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
					fprintf(output, "pushs tf@return\n");
				fprintf(output, "popframe\n");
			}

			break;
		case token_val_integer:
		case token_val_double:
			numOp++;
			// if (TvalToKeyword(active_token->type) != type  && type != token_boolean)
			// {
			// 	name = convert(TvalToKeyword(active_token->type), type, active_token->value, NULL);
			// 	fprintf(output, "pushs lf@%s\n", name);
			// 	free(name);
			// }
			// else
			// {
				fprintf(output, "pushs ");
				printTokenVal();
			// }
			break;
		case token_val_string:
			name = active_token->value.c;
			get_token_free();
			if (type != token_string)
				raise_error(TYPE_ERROR, "Wrong type in expression\n");
			else if (active_token->type == token_eol)
			{
				fprintf(output, "pushs string@%s\n", name);
				free(name);
			}
			else if (istype(active_token->type))
				raise_error(TYPE_ERROR, "Wrong type in expression\n");
			else
				raise_error(SYNTAX_ERROR, "Wrong string initialisation syntax expected EOL\n");
			return;
		default:
			if (istype(active_token->type))
				raise_error(TYPE_ERROR, "Wrong type in expression\n");
			else
				raise_error(SYNTAX_ERROR, "Wrong expression syntax expected EOL\n");
			return;
		}
		get_token_free();
	}
	while (s->priority < STACK_STOPPER)
	{
		if (execOp(&s, &numOp).priority == STACK_STOPPER)
			return;
	}
	if (numOp != 1)
	{
		raise_error(SYNTAX_ERROR, "Wrong expression syntax\n");
		return;
	}
	free(sPop(&s));
}