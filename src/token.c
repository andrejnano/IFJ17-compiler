#include <stdio.h>
#include <stdlib.h>
#include "scanner.h"
#include "token.h"
#include "errors.h"

void raise_error(int error_code, const char *error_message);
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
  bool istype(int type)
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
  void printTokenType(FILE *output_file, int type) {
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
      @TODO dilema.. move aj ked je to nespravne ? alebo len vratit false a 
                      zostat na tom istom tokene
   *****************************************************************************/
  
  bool match(int token_type, int expected_token_type)
  {
  
     // safeguard flag for reading tokens after EOF
     // if EOF token occurs, this forbids further token reading
     static bool eof_token_found = false;
  
     
     if (eof_token_found)
     {
        // dont try to get to the next token
        raise_error(SYNTAX_ERROR, "Unexpected END OF FILE ! ! !");
        return false;
     }
     else 
     {
        // if no EOF token found yet, proceed normally
  
        // is the active_token the expected one ?
        if (active_token->type == expected_token_type) 
        {
           free(active_token);
           active_token = get_next_token(input);
  
           if (active_token->type == token_eof) // eof check
              eof_token_found = true;
  
           
           return true;
        }
        else
        {
           raise_error(SYNTAX_ERROR, " expected ");
           printTokenType(stderr, expected_token_type);
           free(active_token);
           active_token = get_next_token(input);
  
           if (active_token->type == token_eof) // eof check
              eof_token_found = true;
  
           return false;
        }
     }
  }
  