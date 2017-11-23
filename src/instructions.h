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

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

typedef enum{
  i_null, //no variable or constant - just following string is copied
  i_gf, //global frame
  i_lf, //local frame
  i_tf, //temporary frame
  i_int,  //integer constant
  i_fl,  //float constant
  i_str,  //string
  i_bool //bool value
}t_const;

typedef struct instruction {
  char *inst_type;          //instruction (for example "MOVE" or "DEFVAR")
  t_const first_type;    //type od first operand
  char *first;              //first operand
  t_const second_type;
  char *second;
  t_const third_type;
  char *third;
  struct instruction *next; //pointer to next instruction
} t_inst;

t_inst *first_inst;
t_inst *last_inst;

void add_inst(char *inst_type, t_const first_type, char *first,
              t_const second_type, char *second, t_const third_type, char *third);
void generate_code(FILE* output_file);
void free_inst_list();

#endif
