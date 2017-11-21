#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "token.h"
#include "errors.h"


extern FILE* source_code;

/*
  * \brief Reads token from input and dispose current token
  */
void get_token_free()
{
    if (active_token)
    {
        free(active_token);
    }
    active_token = get_next_token(input);
}

/*
    * \brief Checks if token type is int, double, bool, string
    */
bool is_datatype(int type)
{
    if (type != token_integer && type != token_double &&
        type != token_boolean && type != token_string)
        return false;
    return true;
}

/*
   * \brief Function for Token type from it's int representation
   * \param output_file Place to print
   * \param type Token type to decode
   */
void printTokenType(FILE *output_file, int type)
{
    switch (type)
    {
    case token_blank:
        fprintf(output_file, "token_blank\n");
        break;
    case token_identifier:
        fprintf(output_file, "token_identifier\n");
        break;
    case token_val_integer:
        fprintf(output_file, "token_val_integer\n");
        break;
    case token_val_double:
        fprintf(output_file, "token_val_double\n");
        break;
    case token_val_string:
        fprintf(output_file, "token_val_string\n");
        break;
    case token_op_add:
        fprintf(output_file, "token_op_add\n");
        break;
    case token_op_sub:
        fprintf(output_file, "token_op_sub\n");
        break;
    case token_op_mul:
        fprintf(output_file, "token_op_mul\n");
        break;
    case token_op_div:
        fprintf(output_file, "token_op_div\n");
        break;
    case token_op_mod:
        fprintf(output_file, "token_op_mod\n");
        break;
    case token_op_lt:
        fprintf(output_file, "token_op_lt\n");
        break;
    case token_op_gt:
        fprintf(output_file, "token_op_gt\n");
        break;
    case token_op_le:
        fprintf(output_file, "token_op_le\n");
        break;
    case token_op_ge:
        fprintf(output_file, "token_op_ge\n");
        break;
    case token_op_eq:
        fprintf(output_file, "token_op_eq\n");
        break;
    case token_op_ne:
        fprintf(output_file, "token_op_ne\n");
        break;
    case token_semicolon:
        fprintf(output_file, "token_semicolon\n");
        break;
    case token_comma:
        fprintf(output_file, "token_comma\n");
        break;
    case token_lbrace:
        fprintf(output_file, "token_lbrace\n");
        break;
    case token_rbrace:
        fprintf(output_file, "token_rbrace\n");
        break;
    case token_and:
        fprintf(output_file, "token_and\n");
        break;
    case token_as:
        fprintf(output_file, "token_as\n");
        break;
    case token_asc:
        fprintf(output_file, "token_asc\n");
        break;
    case token_boolean:
        fprintf(output_file, "token_boolean\n");
        break;
    case token_chr:
        fprintf(output_file, "token_chr\n");
        break;
    case token_continue:
        fprintf(output_file, "token_continue\n");
        break;
    case token_declare:
        fprintf(output_file, "token_declare\n");
        break;
    case token_dim:
        fprintf(output_file, "token_dim\n");
        break;
    case token_do:
        fprintf(output_file, "token_do\n");
        break;
    case token_double:
        fprintf(output_file, "token_double\n");
        break;
    case token_else:
        fprintf(output_file, "token_else\n");
        break;
    case token_elseif:
        fprintf(output_file, "token_elseif\n");
        break;
    case token_end:
        fprintf(output_file, "token_end\n");
        break;
    case token_exit:
        fprintf(output_file, "token_exit\n");
        break;
    case token_false:
        fprintf(output_file, "token_false\n");
        break;
    case token_for:
        fprintf(output_file, "token_for\n");
        break;
    case token_function:
        fprintf(output_file, "token_function\n");
        break;
    case token_if:
        fprintf(output_file, "token_if\n");
        break;
    case token_input:
        fprintf(output_file, "token_input\n");
        break;
    case token_integer:
        fprintf(output_file, "token_integer\n");
        break;
    case token_length:
        fprintf(output_file, "token_length\n");
        break;
    case token_loop:
        fprintf(output_file, "token_loop\n");
        break;
    case token_next:
        fprintf(output_file, "token_next\n");
        break;
    case token_not:
        fprintf(output_file, "token_not\n");
        break;
    case token_or:
        fprintf(output_file, "token_or\n");
        break;
    case token_print:
        fprintf(output_file, "token_print\n");
        break;
    case token_return:
        fprintf(output_file, "token_return\n");
        break;
    case token_scope:
        fprintf(output_file, "token_scope\n");
        break;
    case token_shared:
        fprintf(output_file, "token_shared\n");
        break;
    case token_static:
        fprintf(output_file, "token_static\n");
        break;
    case token_string:
        fprintf(output_file, "token_string\n");
        break;
    case token_substr:
        fprintf(output_file, "token_substr\n");
        break;
    case token_then:
        fprintf(output_file, "token_then\n");
        break;
    case token_true:
        fprintf(output_file, "token_true\n");
        break;
    case token_while:
        fprintf(output_file, "token_while\n");
        break;
    case token_eol:
        fprintf(output_file, "token_eol\n");
        break;
    case token_eof:
        fprintf(output_file, "token_eof\n");
        break;
    }
}
/*
    * \brief Print value of token literal to output file with prefix:
    * float@ int@ ...
    */
void printTokenVal(void)
{
    switch (active_token->type)
    {
    case token_val_double:
        fprintf(output, "float@%lf\n", active_token->value.d);
        break;
    case token_val_integer:
        fprintf(output, "int@%d\n", active_token->value.i);
        break;
    case token_val_string:
        fprintf(output, "string@%s\n", active_token->value.c);
        break;
    default:
        fprintf(stderr, "Error at line %d cannot print this type\n", active_token->line);
        return;
    }
}


/******************************************************************************
    TOKEN MATCH 
 *****************************************************************************/

/*
	 * \brief	Checks if the current token is the expected one.
	 * 			If it is. It either frees the token and its memory
	 * 			or just moves on. In case of EOF, the function gets
	 * 			blocked for futher token getting. Just prints the error message.
	 * \param	expected_token_type The expected token by syntax
	 * \return	bool value, wether the token was found or not
	 * 			
	 * 			@TODO refactor a bit
	 */

bool match(token_type expected_token_type)
{

    // guard flag that, if switched on, forbids further reading from scanner
    // and therefore prevents wrong memory access.
    // switches on when at least one of these events happen:
    //	1) the scanner returns false, signaling erro
    //  2) the token is EOF, which is unexpected until the end of program
    // --------------------------------------------------------------------
    static bool block_scanner = false;

    if (block_scanner)
    {
        raise_error(E_SYNTAX, "Unexpected END OF FILE ! ! !");
        return false;

        // not exactly right, @TODO change this
        if (expected_token_type == token_eof)
        {
            return true;
        }
    }
    else
    {
        // if the token exists and its type is the expected type
        if (active_token && active_token->type == expected_token_type)
        {
            // if the token is string, free the char array
            if (active_token->type == token_val_string)
                free(active_token->value.c);

            // free the memory allocated by token
            free(active_token);

            // if error occurs in scanner,
            if (get_next_token(source_code, active_token) == false || active_token->type == token_eof)
            {
                block_scanner = true;
                printf("\n[DEBUG]: from match() > error in scanner or EOF token.\n"); // remove later
            }

            return true;
        }
        else
        {

            // if the token is string, free the char array
            if (active_token->type == token_val_string)
                free(active_token->value.c);

            // free the memory allocated by token
            free(active_token);

            // if error occurs in scanner,
            if (get_next_token(source_code, active_token) == false || active_token->type == token_eof)
            {
                block_scanner = true;
                printf("\n[DEBUG]: from match() > error in scanner or EOF token.\n"); // remove later
            }

            return false;
        }
    }
}
