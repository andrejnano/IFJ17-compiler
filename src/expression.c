#include "expression.h"
#include "symtable.h"
#include "errors.h"
#include "token.h"
#include "parser.h"
#include "instructions.h"




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
        add_inst("DEFVAR", i_lf, *var ,i_null,NULL,i_null,NULL);
	//fprintf(output_code, "defvar lf@%s\n", *var);
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
			raise_error(E_SEM_TYPE, "Wrong type for conversion\n");
			return NULL;
		}
		generateName(&tmpVar);
		if (!frame)
			add_inst("FLOAT2R2EINT", i_lf, tmpVar, i_fl , d2s(val.d), i_null,NULL);
			//fprintf(output_code, "float2r2eint lf@%s float@%g\n", tmpVar, val.d);
		else
			add_inst("FLOAT2R2EINT", i_lf, tmpVar, fr2in(frame), val.c, i_null,NULL);
			//fprintf(output_code, "float2r2eint lf@%s %s@%s\n", tmpVar, frame, val.c);
	break;
	case  token_double:
		if (inType != token_integer)
		{
			raise_error(E_SEM_TYPE, "Wrong type for conversion\n");
			return NULL;
		}
		generateName(&tmpVar);
		if (!frame)
			add_inst("INT2FLOAT", i_lf, tmpVar, i_int , i2s(val.i), i_null,NULL);
			//fprintf(output_code, "int2float lf@%s int@%d\n", tmpVar, val.i);
		else
			add_inst("FLOAT2R2EINT", i_lf, tmpVar, fr2in(frame) , val.c, i_null,NULL);
			//fprintf(output_code, "int2float lf@%s %s@%s\n", tmpVar, frame, val.c);
	break;
	default:
		if (istype(active_token->type))
			raise_error(E_SEM_TYPE, "Wrong type for conversion\n");
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
			add_inst("MOVE", i_lf, varName, i_int , "0", i_null,NULL);
			//fprintf(output_code, "move lf@%s int@0\n", varName);
			break;
		case token_double:
			add_inst("MOVE", i_lf, varName, i_fl , "0.0", i_null,NULL);
			//fprintf(output_code, "move lf@%s float@0.0\n", varName);
			break;
		case token_string:
			add_inst("MOVE", i_lf, varName, i_str , "", i_null,NULL);
			//fprintf(output_code, "move lf@%s string@\n", varName);
			break;
		case token_boolean:
			add_inst("MOVE", i_lf, varName, i_bool , "false", i_null,NULL);
			//fprintf(output_code, "move lf@%s bool@false\n", varName);
			break;
		default :
			raise_error(SYNTAX_ERROR, "Expecet type int, double ...\n");
	}
}

/*
 * \brief Converting values stored at stack from type T1 to old_type
 * \param new_type Type of second value in stack
 * \param last type type of last inserted value in stack
 * \param by prierity convert to type with more priority
 *  if 0 convert from old type to new type
 */
void converts(int new_type, int *old_type, bool byPriority)
{
	if (new_type == *old_type)
		return;
	if (istype(new_type) && istype(new_type))
	{
		if (new_type == token_double && *old_type == token_integer)
		{
			if (byPriority)
			{
				char *temp = "tmp";
				generateName(&temp);
			        add_inst("POPS", i_lf, temp, i_null,NULL,i_null,NULL);
			        add_inst("INT2FLOATS", i_null,NULL,i_null,NULL,i_null,NULL);
			        add_inst("PUSHS", i_lf, temp, i_null,NULL,i_null,NULL);
				//fprintf(output_code, "pops lf@%s\n", temp);
				//fprintf(output_code, "int2floats\n");
				//fprintf(output_code, "pushs lf@%s\n", temp);
				*old_type = token_double;
			}
			else
			{
			        add_inst("FLOAT2R2EINTS", i_null,NULL,i_null,NULL,i_null,NULL);
				//fprintf(output_code, "float2r2eints\n");
				*old_type = token_integer;
			}
		}
		else if (*old_type == token_double && new_type == token_integer)
		{
			add_inst("INT2FLOATS", i_null,NULL,i_null,NULL,i_null,NULL);
			//fprintf(output_code, "int2floats\n");
			*old_type = token_double;
		}
		else
		{
			raise_error(E_SEM_TYPE, "Wrong type for conversion\n");
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
		add_inst("INT2FLOATS", i_null,NULL,i_null,NULL,i_null,NULL);
		//fprintf(output_code, "int2floats\n");
		*OP2 = token_double;
	}
	else if (OP1 == token_integer && *OP2 == token_double)
	{
		char *temp1 = "tmp";
		generateName(&temp1);
		add_inst("POPS", i_lf, temp1, i_null,NULL,i_null,NULL);
		add_inst("INT2FLOATS", i_null,NULL,i_null,NULL,i_null,NULL);
		add_inst("PUSHS", i_lf, temp1, i_null,NULL,i_null,NULL);
		//fprintf(output_code, "pops lf@%s\n", temp1);
		//fprintf(output_code, "int2floats\n");
		//fprintf(output_code, "pushs lf@%s\n", temp1);
		*OP2 = token_double;
	}
	else
	{
		raise_error(E_SEM_TYPE, "Wrong type for comparison\n");
	}
}
/*
 * \brief Function for executing operation from top of stack
 * \param s Stack of operations
 * \param numOp Number of operators in stack
 * \return Executed operation, in case of error operation
 * with priority STACK_STOPPER
 */
tStack execOp (tStack **s, int *numOp, int *old_type)
{
	bool int_div;
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
			if (*old_type == token_string)
			{
				generateName(&tmpName1);
				generateName(&tmpName2);
				add_inst("POPS", i_lf, tmpName1, i_null,NULL,i_null,NULL);
				add_inst("POPS", i_lf, tmpName2, i_null,NULL,i_null,NULL);
				add_inst("CONCAT", i_lf, tmpName1, i_lf, tmpName2, i_lf, tmpName1);
				add_inst("PUSHS", i_lf, tmpName1, i_null,NULL,i_null,NULL);
				//fprintf(output_code, "pops lf@%s\n", tmpName1);
				//fprintf(output_code, "pops lf@%s\n", tmpName2);
				//fprintf(output_code, "concat lf@%s lf@%s lf@%s\n", tmpName1, tmpName2, tmpName1);
				//fprintf(output_code, "pushs lf@%s\n", tmpName1);
				break;
			}
			converts(operation->lOperandType, old_type, 1);
			add_inst("ADDS", i_null,NULL,i_null,NULL,i_null,NULL);
			//fprintf(output_code,"adds\n");
		break;
		case token_op_sub:
			converts(operation->lOperandType, old_type, 1);
			if (*old_type == token_string)
				raise_error(E_SEM_TYPE, "Invalid operation for strings\n");
			add_inst("SUBS", i_null,NULL,i_null,NULL,i_null,NULL);
			//fprintf(output_code,"subs\n");
		break;  
		case token_op_mul:
			converts(operation->lOperandType, old_type, 1);
			if (*old_type == token_string)
				raise_error(E_SEM_TYPE, "Invalid operation for strings\n");
			add_inst("MULS", i_null,NULL,i_null,NULL,i_null,NULL);
			//fprintf(output_code,"muls\n");
		break;  
		case token_op_div:
			int_div = false;
			if (operation->lOperandType == token_integer)
			{
				add_inst("INT2FLOATS", i_null, NULL, i_null,NULL,i_null,NULL);
				if (*old_type == token_integer)
					int_div = true;
			}
			converts(token_double, old_type, 1);
			if (*old_type == token_string)
				raise_error(E_SEM_TYPE, "Invalid operation for strings\n");
			add_inst("DIVS", i_null,NULL,i_null,NULL,i_null,NULL);
			if (int_div)
				add_inst("FLOAT2INTS", i_null,NULL,i_null,NULL,i_null,NULL);
			//fprintf(output_code,"divs\n");
		break;  
		case token_op_mod:
			converts(operation->lOperandType, old_type, 1);
			generateName(&tmpName1);
			generateName(&tmpName2);
			if (operation->lOperandType == token_integer)
				add_inst("INT2FLOATS", i_null, NULL, i_null,NULL,i_null,NULL);
			add_inst("POPS", i_lf, tmpName1, i_null,NULL,i_null,NULL);
			if (*old_type == token_integer)
				add_inst("INT2FLOATS", i_null, NULL, i_null,NULL,i_null,NULL);
			add_inst("POPS", i_lf, tmpName2, i_null,NULL,i_null,NULL);
			add_inst("PUSHS", i_lf, tmpName2, i_null,NULL,i_null,NULL);
			add_inst("PUSHS", i_lf, tmpName2, i_null,NULL,i_null,NULL);
			add_inst("PUSHS", i_lf, tmpName1, i_null,NULL,i_null,NULL);
			add_inst("DIVS", i_null,NULL,i_null,NULL,i_null,NULL);
			add_inst("PUSHS", i_lf, tmpName1, i_null,NULL,i_null,NULL);
			add_inst("MULS", i_null,NULL,i_null,NULL,i_null,NULL);
			add_inst("SUBS", i_null,NULL,i_null,NULL,i_null,NULL);
			//fprintf(output_code, "pops lf@%s\n", tmpName1);
			//fprintf(output_code, "pops lf@%s\n", tmpName2);
			//fprintf(output_code, "pushs lf@%s\n", tmpName2);
			//fprintf(output_code, "pushs lf@%s\n", tmpName2);
			//fprintf(output_code, "pushs lf@%s\n", tmpName1);
			//fprintf(output_code, "divs\n");
			//fprintf(output_code, "pushs lf@%s\n", tmpName1);
			//fprintf(output_code, "muls\n");
			//fprintf(output_code, "subs\n");
			free(tmpName1);
			free(tmpName2);
			add_inst("DIVS", i_null,NULL,i_null,NULL,i_null,NULL);
			add_inst("FLOAT2INTS", i_null, NULL, i_null,NULL,i_null,NULL);
			//fprintf(output_code,"divs\n");
		break;
		case token_op_eq:
			testCmpOps(operation->lOperandType, old_type);
			add_inst("EQS", i_null,NULL,i_null,NULL,i_null,NULL);
			//fprintf(output_code, "eqs\n");
			*old_type = token_boolean;
		break;
		case token_op_lt:
			testCmpOps(operation->lOperandType, old_type);
			add_inst("LTS", i_null,NULL,i_null,NULL,i_null,NULL);
			//add_inst("PUSHS*", i_null,NULL,i_null,NULL,i_null,NULL);
			//fprintf(output_code, "lts\npushs");
			*old_type = token_boolean;
		break;
		case token_op_le:
			testCmpOps(operation->lOperandType, old_type);
			generateName(&tmpName1);
			generateName(&tmpName2);
			add_inst("POPS", i_lf, tmpName1, i_null,NULL,i_null,NULL);
			add_inst("POPS", i_lf, tmpName2, i_null,NULL,i_null,NULL);
			add_inst("PUSHS", i_lf, tmpName2, i_null,NULL,i_null,NULL);
			add_inst("PUSHS", i_lf, tmpName1, i_null,NULL,i_null,NULL);
			add_inst("LTS", i_null,NULL,i_null,NULL,i_null,NULL);
			add_inst("PUSHS", i_lf, tmpName2, i_null,NULL,i_null,NULL);
			add_inst("PUSHS", i_lf, tmpName1, i_null,NULL,i_null,NULL);
			add_inst("EQS", i_null,NULL,i_null,NULL,i_null,NULL);
			add_inst("ORS", i_null,NULL,i_null,NULL,i_null,NULL);
			//fprintf(output_code, "pops lf@%s\n", tmpName1);
			//fprintf(output_code, "pops lf@%s\n", tmpName2);
			//fprintf(output_code, "pushs lf@%s\n", tmpName2);
			//fprintf(output_code, "pushs lf@%s\n", tmpName1);
			//fprintf(output_code, "lts\n");
			//fprintf(output_code, "pushs lf@%s\n", tmpName2);
			//fprintf(output_code, "pushs lf@%s\n", tmpName1);
			//fprintf(output_code, "eqs\nors\n");
			*old_type = token_boolean;
		break;
		case token_op_ge:
			testCmpOps(operation->lOperandType, old_type);
			generateName(&tmpName1);
			generateName(&tmpName2);
			add_inst("POPS", i_lf, tmpName1, i_null,NULL,i_null,NULL);
			add_inst("POPS", i_lf, tmpName2, i_null,NULL,i_null,NULL);
			add_inst("PUSHS", i_lf, tmpName2, i_null,NULL,i_null,NULL);
			add_inst("PUSHS", i_lf, tmpName1, i_null,NULL,i_null,NULL);
			add_inst("GTS", i_null,NULL,i_null,NULL,i_null,NULL);
			add_inst("PUSHS", i_lf, tmpName2, i_null,NULL,i_null,NULL);
			add_inst("PUSHS", i_lf, tmpName1, i_null,NULL,i_null,NULL);
			add_inst("EQS", i_null,NULL,i_null,NULL,i_null,NULL);
			add_inst("ORS", i_null,NULL,i_null,NULL,i_null,NULL);
			//fprintf(output_code, "pops lf@%s\n", tmpName1);
			//fprintf(output_code, "pops lf@%s\n", tmpName2);
			//fprintf(output_code, "pushs lf@%s\n", tmpName2);
			//fprintf(output_code, "pushs lf@%s\n", tmpName1);
			//fprintf(output_code, "gts\n");
			//fprintf(output_code, "pushs lf@%s\n", tmpName2);
			//fprintf(output_code, "pushs lf@%s\n", tmpName1);
			//fprintf(output_code, "eqs\nors\n");
			*old_type = token_boolean;
		break;
		case token_op_ne:
			testCmpOps(operation->lOperandType, old_type);
			generateName(&tmpName1);
			generateName(&tmpName2);
			add_inst("POPS", i_lf, tmpName1, i_null,NULL,i_null,NULL);
			add_inst("POPS", i_lf, tmpName2, i_null,NULL,i_null,NULL);
			add_inst("PUSHS", i_lf, tmpName2, i_null,NULL,i_null,NULL);
			add_inst("PUSHS", i_lf, tmpName1, i_null,NULL,i_null,NULL);
			add_inst("LTS", i_null,NULL,i_null,NULL,i_null,NULL);
			add_inst("PUSHS", i_lf, tmpName2, i_null,NULL,i_null,NULL);
			add_inst("PUSHS", i_lf, tmpName1, i_null,NULL,i_null,NULL);
			add_inst("GTS", i_null,NULL,i_null,NULL,i_null,NULL);
			add_inst("ORS", i_null,NULL,i_null,NULL,i_null,NULL);
			//fprintf(output_code, "pops lf@%s\n", tmpName1);
			//fprintf(output_code, "pops lf@%s\n", tmpName2);
			//fprintf(output_code, "pushs lf@%s\n", tmpName2);
			//fprintf(output_code, "pushs lf@%s\n", tmpName1);
			//fprintf(output_code, "lts\n");
			//fprintf(output_code, "pushs lf@%s\n", tmpName2);
			//fprintf(output_code, "pushs lf@%s\n", tmpName1);
			//fprintf(output_code, "gts\nors\n");
			*old_type = token_boolean;
		break;
		case token_op_gt:
			testCmpOps(operation->lOperandType, old_type);
			add_inst("GTS", i_null,NULL,i_null,NULL,i_null,NULL);
			//fprintf(output_code, "gts\n");	
			*old_type = token_boolean;	
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
 * \param funcname Name of function to be called
 */
void NT_CallExpr(Metadata_t *funcMeta, char *funcName)
{
	get_next_token(source_code, active_token);
	match(token_lbrace);
	add_inst("CREATEFRAME", i_null,NULL,i_null,NULL,i_null,NULL);
	//fprintf(output_code, "createframe\n");
	Parameter_t *arg = funcMeta->parameters;
	// read check types and convert arguments for calling function
	while (active_token->type != token_rbrace)
	{
		if (!arg)	// no bracket but still expected argument
		{
			raise_error(E_SEM_DEF, "Too many arguments for function\n");
			return;
		}
		add_inst("DEFVAR", i_tf, arg->name, i_null,NULL,i_null,NULL);
		//fprintf(output_code, "defvar tf@%s\n", arg->name);
		if (active_token->type == token_identifier)
		{
			char *id = active_token->value.c;
			Metadata_t *tmpMeta;
			if ((tmpMeta = stl_search(variables, id)))
			{
				if (tmpMeta->type == funcMeta->type)
				{
					add_inst("MOVE", i_tf, arg->name, i_lf, id, i_null,NULL);
					//fprintf(output_code, "move tf@%s lf@%s\n", arg->name, id);
				}
				else
				{
					char *name = convert(tmpMeta->type, arg->type, active_token->value, "lf");
					add_inst("MOVE", i_tf, arg->name, i_lf, name, i_null,NULL);
					//fprintf(output_code, "move tf@%s lf@%s\n", arg->name, name);
					free(name);
				}
				
			}
			// if ((tmpMeta = stl_search(functions, id)))
			// {
			// 	NT_CallExpr(tmpMeta, id);
			// }
			else
			{
				raise_error(E_SEM_DEF, "Use of undeclared\n");
				return;
			}
		}
		// tvalToKeyword returns corresponding keyword to literal for example token_val_integer -> token_integer
		// it is used because types are in structures stored as keyword for example token_integer
		else if (TvalToKeyword(active_token->type) == arg->type)
		{
			add_inst("MOVE", i_tf, arg->name, i_null, NULL, i_null,NULL);
			//fprintf(output_code, "move tf@%s ", arg->name);
			printTokenVal();
		}
		else
		{
			char *name = convert(TvalToKeyword(active_token->type), arg->type, active_token->value, NULL);
			add_inst("MOVE", i_tf, arg->name, i_lf, name, i_null,NULL);
			//fprintf(output_code, "move tf@%s lf@%s\n", arg->name, name);
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
		raise_error(E_SEM_TYPE, "Too few arguments for function\n");
		return;
	}
	add_inst("PUSHFRAME", i_null,NULL,i_null,NULL,i_null,NULL);
	add_inst("CALL", i_null, funcName, i_null,NULL,i_null,NULL);
	//fprintf(output_code, "pushframe\ncall %s\n", funcName);
}

/*
 * \brief Function for generating code for expression evaluation
 * \param type Expected expression type
 */
void NT_Expr(int type)
{
	tStack *s;
	sInit(&s);
	tStack insert; // pouzijem insert ako zarazku do stacku
	insert.priority = STACK_STOPPER;
	insert.type = STACK_STOPPER;
	sPush(&s, &insert);
	int numOp = 0, old_type = 0;
	Metadata_t *var;
	int i = 0;
	while (active_token->type != token_eol)
	{

		switch (active_token->type)
		{
		case token_op_add:
		case token_op_sub:
			insert.type = active_token->type;
         insert.priority = 3;
         insert.lOperandType = old_type;
			while (s && s->priority && s->priority <= insert.priority)
				if (execOp(&s, &numOp, &old_type).priority == STACK_STOPPER)
					return;
			sPush(&s, &insert);
			break;
		case token_op_div:
		case token_op_mul:
			insert.type = active_token->type;
			insert.priority = 1;
         insert.lOperandType = old_type;
			while (s && s->priority && s->priority <= insert.priority)
				if (execOp(&s, &numOp, &old_type).priority == STACK_STOPPER)
					return;
			sPush(&s, &insert);

			break;
		case token_op_mod:
			insert.type = active_token->type;
			insert.priority = 2;
         insert.lOperandType = old_type;
			while (s && s->priority && s->priority <= insert.priority)
				if (execOp(&s, &numOp, &old_type).priority == STACK_STOPPER)
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
				raise_error(E_SEM_TYPE, "Error at line %d : Expected boolean\n");
				return;
			}
			insert.lOperandType = old_type;
			insert.type = active_token->type;
			insert.priority = 4;
			while (s && s->priority && s->priority <= insert.priority)
				if (execOp(&s, &numOp, &old_type).priority == STACK_STOPPER)
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
				if (execOp(&s, &numOp, &old_type).priority == STACK_STOPPER)
					return;
			}
			free(sPop(&s));
			break;
		case token_identifier:
		 numOp++;
			if ((var = stl_search(variables, active_token->value.c)))
			{
            	old_type = var->type;
                add_inst("PUSHS", i_lf, active_token->value.c, i_null,NULL,i_null,NULL);
				//fprintf(output_code, "pushs lf@%s\n", active_token->value.c);
			}
			else if ((var = stl_search(functions, active_token->value.c)))
			{
            	old_type = var->type;
				NT_CallExpr(var, active_token->value.c);
				add_inst("POPFRAME", i_null,NULL,i_null,NULL,i_null,NULL);
				add_inst("PUSHS", i_tf, "%retval", i_null,NULL,i_null,NULL);
				//fprintf(output_code, "pushs tf@return\n");
				//fprintf(output_code, "popframe\n");
			}
			else
			{
				raise_error(E_SEM_DEF, "Unknown identifier\n");
			}

			break;
		case token_val_integer:
		case token_val_double:
		case token_val_string:
         numOp++;
			old_type = TvalToKeyword(active_token->type);
                        add_inst("PUSHS", i_null,NULL,i_null,NULL,i_null,NULL); 
			//fprintf(output_code, "pushs ");
			printTokenVal();
			break;
		default:
			if (istype(active_token->type))
				raise_error(E_SEM_TYPE, "Wrong type in expression\n");
			if (!i)
			{
				raise_error(SYNTAX_ERROR, "Wrong syntax of expression\n");
			}
			while (s && s->priority < STACK_STOPPER)
			{
				if (execOp(&s, &numOp, &old_type).priority == STACK_STOPPER)
					return;
			}
			if (numOp != 1)
			{
				raise_error(SYNTAX_ERROR, "Wrong expression syntax\n");
				return;
			}
			if (type && type != old_type)
				converts(old_type, &type, 0);
			free(sPop(&s));		
			return;
		}

		get_next_token(source_code, active_token);
		i++;
	}
	while (s && s->priority < STACK_STOPPER)
	{
		if (execOp(&s, &numOp, &old_type).priority == STACK_STOPPER)
			return;
	}
	if (numOp != 1)
	{
		raise_error(SYNTAX_ERROR, "Wrong expression syntax\n");
		return;
	}
	if (type && type != old_type)
		converts(old_type, &type, 0);
	free(sPop(&s));
}
