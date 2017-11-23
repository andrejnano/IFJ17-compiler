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

void print_const_type(FILE* output_file, t_const type)
{
  switch(type)
  {
    case i_null:
      return;  
    case i_gf:
      fprintf(output_file, "GF@");
      return;
    case i_lf:
      fprintf(output_file, "LF@");
      return;
    case i_tf:
      fprintf(output_file, "TF@");
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
    default:
      fprintf(output_file, "*ERR*@");
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
      print_const_type(output_file, actual->first_type);
      fprintf(output_file, " %s",actual->first); 
    }
    if(actual->second)
    {
      print_const_type(output_file, actual->third_type);
      fprintf(output_file, " %s",actual->second); 
    }
    if(actual->third)
    {
      print_const_type(output_file, actual->third_type);
      fprintf(output_file, " %s",actual->third); 
    }
    fprintf(output_file, "\n");
    actual = actual->next;
  }
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
