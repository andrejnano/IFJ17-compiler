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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "instructions.h"
#include "token.h"

/**
 * Converts integer value to string, returns pointer to string
 */
char* i2s(int val)
{
  char* str;
  int len = snprintf(NULL, 0, "%i", val);
  str = malloc(sizeof(char) * (len+1));
  sprintf(str, "%i", val);
  return str;
}

/**
 * Converts double value to string, returns pointer to string
 */
char* d2s(double val)
{
  char* str;
  int len = snprintf(NULL, 0, "%g", val);
  str = malloc(sizeof(char) * (len+1));
  sprintf(str, "%g", val);
  return str;
}

/**
 * Converts int to data type string, returns pointer to string
 */
char* i2type(int val)
{
  char* str;
  str = malloc(sizeof(char) * 6);
  if(val == token_integer)
  {
    str = "int";
  }
  else if(val == token_double)
  {
    str = "float";
  }
  else if(val == token_boolean)
  {
    str = "bool";
  }
  else if(val == token_string)
  {
    str = "string";
  }
  else
  {
    str = "-ERR-";
  }
  return str;
}

/**
 * converts frame written in string to instruction type 
 */
t_const fr2in(char *frame)
{
  if(frame == NULL)
  {
    return i_str;
  }
  else if(strcmp(frame,"lf") == 0)
  {
    return i_lf;
  }
  else if(strcmp(frame,"gf") == 0)
  {
    return i_gf;
  }
  else if(strcmp(frame,"tf") == 0)
  {
    return i_tf;
  }
  return i_null;
}

/**
 * Add new instruction at bottom of list
 * @param type - instruction as string, for example "MOVE" or "DEFVAR"
 * @param first, second, third - pointers to symbols
 */
void add_inst(char *inst_type, t_const first_type, char *first,
              t_const second_type, char *second, t_const third_type, char *third)
{
  t_inst *i = malloc(sizeof(t_inst));
  i->inst_type = malloc(strlen(inst_type)*sizeof(char));
  strcpy(i->inst_type, inst_type);
  if(first == NULL)
  {
    i->first_type = i_null;
    i->first = NULL;
  }
  else
  {
    i->first_type = first_type;
    i->first = malloc(strlen(first)*sizeof(char));
    strcpy(i->first, first);
  }
  if(second == NULL)
  {
    i->second_type = i_null;
    i->second= NULL;
  }
  else
  {
    i->second_type = second_type;
    i->second = malloc(strlen(second)*sizeof(char));
    strcpy(i->second, second);
  }
  if(third == NULL)
  {
    i->third_type = i_null;
    i->third = NULL;
  }
  else
  {
    i->third_type = third_type;
    i->third = malloc(strlen(third)*sizeof(char));
    strcpy(i->third, third);
  }
  i->next = NULL;
  if(last_inst != NULL)
  {
    last_inst->next = i;
    last_inst = i;
  }
  else
  {
    first_inst = i;
    last_inst = i;
  }
}

/**
 * Add new operand to last instruction
 */
void add_op_to_last_inst(t_const type, char *value)
{
  if(last_inst->first == NULL)
  {
    last_inst->first_type = type;
    last_inst->first = malloc(strlen(value)*sizeof(char));
    strcpy(last_inst->first, value);
  }
  else if(last_inst->second == NULL)
  {
    last_inst->second_type = type;
    last_inst->second = malloc(strlen(value)*sizeof(char));
    strcpy(last_inst->second, value);
  }
  else if(last_inst->third == NULL)
  {
    last_inst->third_type = type;
    last_inst->third = malloc(strlen(value)*sizeof(char));
    strcpy(last_inst->third, value);
  }
}

void print_const_type(FILE* output_file, t_const type)
{
  switch(type)
  {
    case i_null:
      return;  
    case i_gf:
      fprintf(output_file, "gf@");
      return;
    case i_lf:
      fprintf(output_file, "lf@");
      return;
    case i_tf:
      fprintf(output_file, "tf@");
      return;
    case i_int:
      fprintf(output_file, "int@");
      return;
    case i_fl:
      fprintf(output_file, "float@");
      return;
    case i_str:
      fprintf(output_file, "string@");
      return;
    case i_bool:
      fprintf(output_file, "bool@");
      return;
    case i_end:
      fprintf(output_file, "end*");
      return;
    default:
      fprintf(output_file, "-ERR-@");
      return;
  }
}

void generate_code(FILE* output_file)
{
  t_inst *actual = first_inst;
  while(actual != NULL)
  {
    fprintf(output_file, "%s", actual->inst_type);
    if(actual->first)
    {
      fprintf(output_file, " ");
      print_const_type(output_file, actual->first_type);
      fprintf(output_file, "%s",actual->first); 
    }
    if(actual->second)
    {
      fprintf(output_file, " ");
      print_const_type(output_file, actual->second_type);
      fprintf(output_file, "%s",actual->second); 
    }
    if(actual->third)
    {
      fprintf(output_file, " ");
      print_const_type(output_file, actual->third_type);
      fprintf(output_file, "%s",actual->third); 
    }
    fprintf(output_file, "\n");
    actual = actual->next;
  }
}

void generate_base(FILE* output_file)
{
    fprintf(output_file, "\
.IFJcode17\n\
CREATEFRAME\n\
PUSHFRAME\n\
JUMP $main\n");
}

bool builtin_length_used = false;
bool builtin_substr_used = false;
bool builtin_asc_used = false;
bool builtin_chr_used = false;

void generate_builtin(FILE* output_file)
{
    //if Lenght used
    if(builtin_length_used == true)
    {
      fprintf(output_file, "\
LABEL Length\n\
DEFVAR lf@%%retval\n\
STRLEN lf@%%retval lf@s\n\
RETURN\n");
    }
    //TODO generate next built-in functions
    //if Asc used
    if(builtin_asc_used == true)
    {
      fprintf(output_file, "\
LABEL Asc\n\
CREATEFRAME\n\
DEFVAR lf@%%retval\n\
DEFVAR lf@a*cmp\n\
LT lf@a*cmp lf@i int@0\n\
JUMPIFEQ els*a0a bool@false lf@a*cmp\n\
MOVE lf@%%retval int@0\n\
WRITE string@ret0a\n\
RETURN\n\
JUMP end*els*a0a\n\
LABEL els*a0a\n\
DEFVAR lf@len\n\
STRLEN lf@len lf@s\n\
GT lf@a*cmp lf@len lf@i\n\
JUMPIFNEQ els*a1a bool@false lf@a*cmp\n\
MOVE lf@%%retval int@0\n\
WRITE string@ret0b\n\
RETURN\n\
JUMP end*els*a1a\n\
LABEL els*a1a\n\
STRI2INT lf@%%retval lf@s lf@i\n\
LABEL end*els*a1a\n\
LABEL end*els*a0a\n\
RETURN\n");
    }
    //if Chr used
    if(builtin_chr_used == true)
    {
      fprintf(output_file, "\
LABEL Chr\n\
CREATEFRAME\n\
DEFVAR lf@%%retval\n\
DEFVAR lf@c*cmp\n\
LT lf@c*cmp lf@i int@0\n\
JUMPIFEQ els*a0c bool@false lf@c*cmp\n\
MOVE lf@%%retval int@0\n\
WRITE string@ret0a\n\
RETURN\n\
JUMP end*els*a0c\n\
LABEL els*a0c\n\
DEFVAR lf@max\n\
MOVE lf@max int@255\n\
LT lf@c*cmp lf@max lf@i\n\
JUMPIFEQ els*a1c bool@false lf@c*cmp\n\
MOVE lf@%%retval int@0\n\
WRITE string@ret0b\n\
RETURN\n\
JUMP end*els*a1c\n\
LABEL els*a1c\n\
INT2CHAR lf@%%retval lf@i\n\
LABEL end*els*a1c\n\
LABEL end*els*a0c\n\
RETURN\n");
    }
    //if SubStr used
    if(builtin_substr_used == true)
    {
      fprintf(output_file, "\
LABEL SubStr\n\
CREATEFRAME\n\
DEFVAR lf@%%retval\n\
DEFVAR lf@s*cmp\n\
EQ lf@s*cmp lf@s string@\n\
JUMPIFEQ els*a0s bool@false lf@s*cmp\n\
MOVE lf@%%retval string@\n\
WRITE string@retA\n\
RETURN\n\
JUMP end*els*a0s\n\
LABEL els*a0s\n\
GT lf@s*cmp lf@i int@0\n\
JUMPIFNEQ els*a2s bool@false lf@s*cmp\n\
MOVE lf@%%retval string@\n\
WRITE string@retB\n\
RETURN\n\
JUMP end*els*a2s\n\
LABEL els*a2s\n\
DEFVAR lf@len\n\
STRLEN lf@len lf@s\n\
LT lf@s*cmp lf@n int@0\n\
JUMPIFEQ els*a4s bool@false lf@s*cmp\n\
ADD lf@n lf@len int@1\n\
JUMP end*els*a4s\n\
LABEL els*a4s\n\
LT lf@s*cmp lf@len lf@n\n\
JUMPIFEQ els*a6s bool@false lf@s*cmp\n\
ADD lf@n lf@len int@1\n\
JUMP end*els*a6s\n\
LABEL els*a6s\n\
LABEL end*els*a6s\n\
LABEL end*els*a4s\n\
DEFVAR lf@ret\n\
MOVE lf@ret string@\n\
PUSHS string@\n\
POPS lf@ret\n\
DEFVAR lf@act\n\
MOVE lf@act string@\n\
DEFVAR lf@pop$9\n\
SUB lf@i lf@i int@1\n\
ADD lf@len lf@i lf@n\n\
LABEL whl*a8s\n\
LT lf@s*cmp lf@i lf@len\n\
JUMPIFEQ end*whl*a8s bool@false lf@s*cmp\n\
GETCHAR lf@act lf@s lf@i\n\
CONCAT lf@ret lf@ret lf@act\n\
ADD lf@i lf@i int@1\n\
JUMP whl*a8s\n\
LABEL end*whl*a8s\n\
MOVE lf@%%retval lf@ret\n\
LABEL end*els*a2s\n\
LABEL end*els*a0s\n\
RETURN\n");
  }
}

Metadata_t builtin_length_meta;
Metadata_t builtin_substr_meta;
Metadata_t builtin_asc_meta;
Metadata_t builtin_chr_meta;

void set_builtin_meta()
{
  //Length
  builtin_length_meta.type = token_integer;
  builtin_length_meta.is_defined = true;
  builtin_length_meta.is_declared = true;
  builtin_length_meta.parameters = malloc(sizeof(Parameter_t));
  builtin_length_meta.parameters->type = token_string;
  builtin_length_meta.parameters->name = "s";
  builtin_length_meta.parameters->next = NULL;
  //SubStr
  builtin_substr_meta.type = token_string;
  builtin_substr_meta.is_defined = true;
  builtin_substr_meta.is_declared = true;
  builtin_substr_meta.parameters = malloc(sizeof(Parameter_t));
  builtin_substr_meta.parameters->type = token_string;
  builtin_substr_meta.parameters->name = "s";
  builtin_substr_meta.parameters->next = malloc(sizeof(Parameter_t));
  builtin_substr_meta.parameters->next->type = token_integer;
  builtin_substr_meta.parameters->next->name = "i";
  builtin_substr_meta.parameters->next->next = malloc(sizeof(Parameter_t));
  builtin_substr_meta.parameters->next->next->type = token_integer;
  builtin_substr_meta.parameters->next->next->name = "n";
  builtin_substr_meta.parameters->next->next->next = NULL;
  //Asc
  builtin_asc_meta.type = token_integer;
  builtin_asc_meta.is_defined = true;
  builtin_asc_meta.is_declared = true;
  builtin_asc_meta.parameters = malloc(sizeof(Parameter_t));
  builtin_asc_meta.parameters->type = token_string;
  builtin_asc_meta.parameters->name = "s";
  builtin_asc_meta.parameters->next = malloc(sizeof(Parameter_t));
  builtin_asc_meta.parameters->next->type = token_integer;
  builtin_asc_meta.parameters->next->name = "i";
  builtin_asc_meta.parameters->next->next = NULL;  
  //Chr
  builtin_chr_meta.type = token_string;
  builtin_chr_meta.is_defined = true;
  builtin_chr_meta.is_declared = true;
  builtin_chr_meta.parameters = malloc(sizeof(Parameter_t));
  builtin_chr_meta.parameters->type = token_integer;
  builtin_chr_meta.parameters->name = "i";
  builtin_chr_meta.parameters->next = NULL;     
}

void free_inst_list()
{
  while(first_inst != NULL)
  {
    t_inst *tmp = first_inst;
    first_inst = first_inst->next;
    free(tmp);
  }
  last_inst = NULL;
}
