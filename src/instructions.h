/*
    + ------------- +
    | PROJEKT IFJ17 |
    + ------------- +

    Společný projekt IFJ a IAL 2017
        Implementace prekladace imperativního jazyka IFJ17.

    Varianta zadanie:
        Tým 025, varianta I

    Soubor:
        instructions.h

    Autori:
        xmarko15 Peter Marko
        xmechl00 Stanislav Mechl
        xnanoa00 Andrej Nano
        xsvand06 Švanda Jan
*/

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "symtable.h"

extern bool builtin_length_used;
extern bool builtin_substr_used;
extern bool builtin_asc_used;
extern bool builtin_chr_used;

extern Metadata_t builtin_length_meta;
extern Metadata_t builtin_substr_meta;
extern Metadata_t builtin_asc_meta;
extern Metadata_t builtin_chr_meta;

typedef enum{
  i_null, //no variable or constant - just following string is copied
  i_gf, //global frame - generates "gf@" before operand
  i_lf, //local frame
  i_tf, //temporary frame
  i_int,  //integer constant
  i_fl,  //float constant
  i_str,  //string
  i_bool, //bool value
  i_end //generates "end" before operand
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

char* i2s(int val);
char* d2s(double val);
t_const fr2in(char *frame);
char* i2type(int val);
void add_inst(char *inst_type, t_const first_type, char *first,
              t_const second_type, char *second, t_const third_type, char *third);
void add_op_to_last_inst(t_const type, char *value);
void generate_code(FILE* output_file);
void generate_base(FILE* output_file);
void generate_builtin(FILE* output_file);
void set_builtin_meta();
void free_builtin_meta();
void free_inst_list();

#endif
