#include "expression.h"
#include "symtable.h"
#include "errors.h"
#include "token.h"
#include "parser.h"




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
* \brief Generates new unused name
*/
void generateName(char **var)
{
	if (!output_code)
	{
		raise_error(1, "Error : not valid ouput file generateVariable\n");
		return;
	}
	static int count = 0;
	char *before = *var;
	*var = (char *)malloc(sizeof(*var) + 10);
	sprintf(*var,"$%s%u", before, count);
	fprintf(output_code, "defvar lf@%s\n", *var);
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
			fprintf(output_code, "float2r2eint lf@%s float@%g\n", tmpVar, val.d);
		else
			fprintf(output_code, "float2r2eint lf@%s %s@%s\n", tmpVar, frame, val.c);
	break;
	case  token_double:
		if (inType != token_integer)
		{
			raise_error(TYPE_ERROR, "Wrong type for conversion\n");
			return NULL;
		}
		generateName(&tmpVar);
		if (!frame)
			fprintf(output_code, "int2float lf@%s int@%d\n", tmpVar, val.i);
		else
			fprintf(output_code, "int2float lf@%s %s@%s\n", tmpVar, frame, val.c);
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
			fprintf(output_code, "move lf@%s int@0\n", varName);
			break;
		case token_double:
			fprintf(output_code, "move lf@%s float@0.0\n", varName);
			break;
		case token_string:
			fprintf(output_code, "move lf@%s string@\n", varName);
			break;
		case token_boolean:
			fprintf(output_code, "move lf@%s bool@false\n", varName);
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
void NT_Func(bool dec, char **funcName)
{
	match(token_function);
	// saving metadata about function which could be already declared for
	// later comparison
	Metadata_t *oldFuncMeta = stl_search(functions, active_token->value.c);
	if (active_token->type != token_identifier)
	{
		raise_error(SYNTAX_ERROR, "Expectd identifier\n");
		return;
	}
	*funcName = active_token->value.c;
	// creating variable for later storing and comparing with oldFuncMeta
	Metadata_t newFuncMeta;
	newFuncMeta.is_declared = dec;
	newFuncMeta.parameters = NULL;
	get_next_token(source_code, active_token);
	match(token_lbrace);
	// storing arguments to arglist
	Parameter_t *argList = NULL;
	// storing arguments to newFuncMeta.parameters
	while (active_token->type != token_rbrace)
	{	
		Parameter_t *tmpNode = (Parameter_t *)malloc(sizeof (Parameter_t));
		tmpNode->name = active_token->value.c;
		tmpNode->next = NULL;
		get_next_token(source_code, active_token);
		match(token_as);

		if (!istype(active_token->type))
		{
			raise_error( SYNTAX_ERROR, "Expectd type\n");
			return;
		}
		tmpNode->type = active_token->type;
		get_next_token(source_code, active_token);
		// token comma means there will be other argument so continue
		if (active_token->type == token_comma)
		{
			get_next_token(source_code, active_token);
			if (param_list_append(&argList, tmpNode))
				return;
			continue;
		}
		else if (active_token->type != token_rbrace)
		{
			raise_error(SYNTAX_ERROR, "Expected token_rbrace\n");
			return;
		}
		if (param_list_append(&argList, tmpNode))
			return;
	}

	get_next_token(source_code, active_token);
	match(token_as);
	if (!istype(active_token->type))
	{
		raise_error(SYNTAX_ERROR, "Expected type\n");
		return;
	}
	newFuncMeta.type = active_token->type;
	newFuncMeta.parameters = argList;
	if (oldFuncMeta && (oldFuncMeta->is_declared == dec || (!oldFuncMeta->is_declared && dec) || 
		!param_list_cmp(newFuncMeta.parameters, oldFuncMeta->parameters) || newFuncMeta.type != oldFuncMeta->type))
	{
		raise_error( SEM_ERROR, "Redefinition of function %s\n");
		param_list_dispose(argList);
		return;
	}
	// if there were no old metadata insert new
	// otherwise no need for inserting new ones consistence were checked

	if (!oldFuncMeta)
		stl_insert_top(functions, *funcName, &newFuncMeta);
	else
		param_list_dispose(newFuncMeta.parameters);
	get_next_token(source_code, active_token);
	match(token_eol);
	if (!dec)
		fprintf(output_code, "label %s\n", *funcName);
}

/*
 * \brief Converting values stored at stack from type T1 to last_type
 * \param old_type Type of second value in stack
 * \param last type type of last inserted value in stack
 * \param by prierity convert to type with more priority
 *  if 0 convert from old type to new type
 */
void converts(int old_type, int *last_type, bool byPriority)
{
	if (old_type == *last_type)
		return;
	if (istype(old_type) && istype(old_type))
	{
		if (old_type == token_double && *last_type == token_integer)
		{
			if (byPriority)
			{
				char *temp = "tmp";
				generateName(&temp);
				fprintf(output_code, "pops lf@%s\n", temp);
				fprintf(output_code, "int2floats\n");
				fprintf(output_code, "pushs lf@%s\n", temp);
				*last_type = token_double;
			}
			else
			{
				fprintf(output_code, "float2r2eints\n");
				*last_type = token_integer;
			}
		}
		else if (*last_type == token_double && old_type == token_integer)
		{
			fprintf(output_code, "int2floats\n");
			*last_type = token_double;
		}
		else
		{
			raise_error(TYPE_ERROR, "Wrong type for conversion\n");
			return;
		}
	}
	else
	{
		raise_error(SYNTAX_ERROR, "Wrong expression syntax\n");
		return;
	}
}

/*
 * \brief Test if types are valid for comparision
 */
void testCmpOps(int OP1, int *OP2)
{
	if (!istype(OP1) || !istype(*OP2))
	{
		raise_error(SYNTAX_ERROR, "Unexpected token\n");
		return;
	}
	if (OP1 == *OP2)
	{
		return;
	}
	else if (*OP2 == token_integer && OP1 == token_double)
	{
		fprintf(output_code, "int2floats\n");
		*OP2 = token_double;
	}
	else if (OP1 == token_integer && *OP2 == token_double)
	{
		char *temp1 = "tmp";
		generateName(&temp1);
		fprintf(output_code, "pops lf@%s\n", temp1);
		fprintf(output_code, "int2floats\n");
		fprintf(output_code, "pushs lf@%s\n", temp1);
		*OP2 = token_double;
	}
	else
	{
		raise_error(TYPE_ERROR, "Wrong type for comparison\n");
	}
}
/*
 * \brief Function for executing operation from top of stack
 * \param s Stack of operations
 * \param numOp Number of operators in stack
 * \return Executed operation, in case of error operation
 * with priority STACK_STOPPER
 */
tStack execOp (tStack **s, int *numOp, int *last_type)
{
	tStack tmp;
	tmp.priority = STACK_STOPPER;
	
	tStack *operation = sPop(s);
	if (*numOp < 2 && operation->type != token_lbrace)
	{
		raise_error(SYNTAX_ERROR, "Wrong syntax of expression\n");
		tmp.priority = STACK_STOPPER;
		tmp.type = STACK_STOPPER;
		return tmp;
	}
	char *tmpName1 = "tmpName";
	char *tmpName2 = "tmpName";
	switch (operation->type)
	{
		case token_op_add:
			if (*last_type == token_string)
			{
				generateName(&tmpName1);
				generateName(&tmpName2);
				fprintf(output_code, "pops lf@%s\n", tmpName1);
				fprintf(output_code, "pops lf@%s\n", tmpName2);
				fprintf(output_code, "concat lf@%s lf@%s lf@%s\n", tmpName1, tmpName2, tmpName1);
				fprintf(output_code, "pushs lf@%s\n", tmpName1);
				break;
			}
			converts(operation->lOperandType, last_type, 1);
			fprintf(output_code,"adds\n");
		break;
		case token_op_sub:
			converts(operation->lOperandType, last_type, 1);
			if (*last_type == token_string)
				raise_error(SEM_ERROR, "Invalid operation for strings\n");
			fprintf(output_code,"subs\n");
		break;  
		case token_op_mul:
			converts(operation->lOperandType, last_type, 1);
			if (*last_type == token_string)
				raise_error(SEM_ERROR, "Invalid operation for strings\n");
			fprintf(output_code,"muls\n");
		break;  
		case token_op_div:
			converts(operation->lOperandType, last_type, 1);
			if (*last_type == token_string)
				raise_error(SEM_ERROR, "Invalid operation for strings\n");
			fprintf(output_code,"divs\n");
		break;  
		case token_op_mod:
			converts(operation->lOperandType, last_type, 1);
			if (*last_type == token_string)
				raise_error(SEM_ERROR, "Invalid operation for strings\n");
			generateName(&tmpName1);
			generateName(&tmpName2);
			fprintf(output_code, "pops lf@%s\n", tmpName1);
			fprintf(output_code, "pops lf@%s\n", tmpName2);
			fprintf(output_code, "pushs lf@%s\n", tmpName2);
			fprintf(output_code, "pushs lf@%s\n", tmpName2);
			fprintf(output_code, "pushs lf@%s\n", tmpName1);
			fprintf(output_code, "divs\n");
			fprintf(output_code, "pushs lf@%s\n", tmpName1);
			fprintf(output_code, "muls\n");
			fprintf(output_code, "subs\n");
			free(tmpName1);
			free(tmpName2);
			fprintf(output_code,"divs\n");
		break;
		case token_op_eq:
			testCmpOps(operation->lOperandType, last_type);
			fprintf(output_code, "eqs\n");
			*last_type = token_boolean;
		break;
		case token_op_lt:
			testCmpOps(operation->lOperandType, last_type);
			fprintf(output_code, "lts\npushs");
			*last_type = token_boolean;
		break;
		case token_op_le:
			testCmpOps(operation->lOperandType, last_type);
			generateName(&tmpName1);
			generateName(&tmpName2);
			fprintf(output_code, "pops lf@%s\n", tmpName1);
			fprintf(output_code, "pops lf@%s\n", tmpName2);
			fprintf(output_code, "pushs lf@%s\n", tmpName2);
			fprintf(output_code, "pushs lf@%s\n", tmpName1);
			fprintf(output_code, "lts\n");
			fprintf(output_code, "pushs lf@%s\n", tmpName2);
			fprintf(output_code, "pushs lf@%s\n", tmpName1);
			fprintf(output_code, "eqs\nors\n");
			*last_type = token_boolean;
		break;
		case token_op_ge:
			testCmpOps(operation->lOperandType, last_type);
			generateName(&tmpName1);
			generateName(&tmpName2);
			fprintf(output_code, "pops lf@%s\n", tmpName1);
			fprintf(output_code, "pops lf@%s\n", tmpName2);
			fprintf(output_code, "pushs lf@%s\n", tmpName2);
			fprintf(output_code, "pushs lf@%s\n", tmpName1);
			fprintf(output_code, "gts\n");
			fprintf(output_code, "pushs lf@%s\n", tmpName2);
			fprintf(output_code, "pushs lf@%s\n", tmpName1);
			fprintf(output_code, "eqs\nors\n");
			*last_type = token_boolean;
		break;
		case token_op_ne:
			testCmpOps(operation->lOperandType, last_type);
			generateName(&tmpName1);
			generateName(&tmpName2);
			fprintf(output_code, "pops lf@%s\n", tmpName1);
			fprintf(output_code, "pops lf@%s\n", tmpName2);
			fprintf(output_code, "pushs lf@%s\n", tmpName2);
			fprintf(output_code, "pushs lf@%s\n", tmpName1);
			fprintf(output_code, "lts\n");
			fprintf(output_code, "pushs lf@%s\n", tmpName2);
			fprintf(output_code, "pushs lf@%s\n", tmpName1);
			fprintf(output_code, "gts\nors\n");
			*last_type = token_boolean;
		break;
		case token_op_gt:
			testCmpOps(operation->lOperandType, last_type);
			fprintf(output_code, "gts\n");	
			*last_type = token_boolean;	
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
 * \brief Function for generating code for function call
 * \param vars Symbol table list containig metadata about variables
 * \param functions Symbol table list containig metadata about functions
 * \param funcname Name of function to be called
 */
void NT_CallExpr(Metadata_t *funcMeta, char *funcName, SymbolTable_t *localVars)
{
	get_next_token(source_code, active_token);
	match(token_lbrace);
	fprintf(output_code, "createframe\n");
	Parameter_t *arg = funcMeta->parameters;
	// read check types and convert arguments for calling function
	while (active_token->type != token_rbrace)
	{
		if (!arg)	// no bracket but still expected argument
		{
			raise_error(SEM_ERROR, "Too many arguments for function\n");
			return;
		}
		fprintf(output_code, "defvar tf@%s\n", arg->name);
		if (active_token->type == token_identifier)
		{
			char *id = active_token->value.c;
			Metadata_t *tmpMeta;
			if ((tmpMeta = stl_search(localVars, id)))
			{
				if (tmpMeta->type == funcMeta->type)
				{
					fprintf(output_code, "move tf@%s lf@%s\n", arg->name, id);
				}
				else
				{
					char *name = convert(tmpMeta->type, arg->type, active_token->value, "lf");
					fprintf(output_code, "move tf@%s lf@%s\n", arg->name, name);
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
			fprintf(output_code, "move tf@%s ", arg->name);
			printTokenVal();
		}
		else
		{
			char *name = convert(TvalToKeyword(active_token->type), arg->type, active_token->value, NULL);
			fprintf(output_code, "move tf@%s lf@%s\n", arg->name, name);
			free(name);
		}
		arg = arg->next;
		get_next_token(source_code, active_token);
		if (active_token->type != token_comma)
			break;
		get_next_token(source_code, active_token);
	}

	if (arg)
	{
		raise_error(SEM_ERROR, "Too few arguments for function\n");
		return;
	}
	fprintf(output_code, "pushframe\ncall %s\n", funcName);
}

/*
 * \brief Function for generating code for expression evaluation
 * \param vars Symboltable list of variables
 * \param functions Symboltable list of functions
 * \param type Expected expression type
 */
void NT_Expr(int type, SymbolTable_t *localVars)
{
	tStack *s;
	sInit(&s);
	tStack insert; // pouzijem insert ako zarazku do stacku
	insert.priority = STACK_STOPPER;
	insert.type = STACK_STOPPER;
	sPush(&s, &insert);
	int numOp = 0, last_type = 0;
	Metadata_t *var;
	while (active_token->type != token_eol)
	{
		switch (active_token->type)
		{
		case token_op_add:
		case token_op_sub:
			insert.type = active_token->type;
         insert.priority = 3;
         insert.lOperandType = last_type;
			while (s && s->priority && s->priority <= insert.priority)
				if (execOp(&s, &numOp, &last_type).priority == STACK_STOPPER)
					return;
			sPush(&s, &insert);
			break;
		case token_op_div:
		case token_op_mul:
			insert.type = active_token->type;
			insert.priority = 1;
         insert.lOperandType = last_type;
			while (s && s->priority && s->priority <= insert.priority)
				if (execOp(&s, &numOp, &last_type).priority == STACK_STOPPER)
					return;
			sPush(&s, &insert);

			break;
		case token_op_mod:
			insert.type = active_token->type;
			insert.priority = 2;
         insert.lOperandType = last_type;
			while (s && s->priority && s->priority <= insert.priority)
				if (execOp(&s, &numOp, &last_type).priority == STACK_STOPPER)
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
			insert.lOperandType = last_type;
			insert.type = active_token->type;
			insert.priority = 4;
			while (s && s->priority && s->priority <= insert.priority)
				if (execOp(&s, &numOp, &last_type).priority == STACK_STOPPER)
					return;
			sPush(&s, &insert);
			break;
		case token_lbrace:
			insert.type = active_token->type;
			insert.priority = 6;
			sPush(&s, &insert);
			break;
		case token_rbrace:
			while (s && s->type != token_lbrace)
			{
				if (execOp(&s, &numOp, &last_type).priority == STACK_STOPPER)
					return;
			}
			free(sPop(&s));
			break;
		case token_identifier:
         numOp++;
			if ((var = stl_search(localVars, active_token->value.c)))
			{
            	last_type = var->type;
				fprintf(output_code, "pushs lf@%s\n", active_token->value.c);
			}
			else if ((var = stl_search(functions, active_token->value.c)))
			{
            	last_type = var->type;
				NT_CallExpr(var, active_token->value.c, localVars);
				fprintf(output_code, "pushs tf@return\n");
				fprintf(output_code, "popframe\n");
			}

			break;
		case token_val_integer:
		case token_val_double:
		case token_val_string:
         numOp++;
			last_type = TvalToKeyword(active_token->type);
			fprintf(output_code, "pushs ");
			printTokenVal();
			break;
		default:
			if (istype(active_token->type))
				raise_error(TYPE_ERROR, "Wrong type in expression\n");
			else
				raise_error(SYNTAX_ERROR, "Wrong expression syntax expected EOL\n");
			return;
		}
		get_next_token(source_code, active_token);
	}
	while (s && s->priority < STACK_STOPPER)
	{
		if (execOp(&s, &numOp, &last_type).priority == STACK_STOPPER)
			return;
	}
	if (numOp != 1)
	{
		raise_error(SYNTAX_ERROR, "Wrong expression syntax\n");
		return;
	}
	if (type != last_type)
		converts(last_type, &type, 0);
	free(sPop(&s));
}
