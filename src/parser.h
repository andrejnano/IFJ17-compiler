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

#ifndef PARSER_H
#define PARSER_H
#include <stdio.h>

enum ins {
	MOVE,
	CREATEFRAME,
	PUSHFRAME,
	POPFRAME,
	DEFVAR,
	CALL,
	RETURN,
	PUSHS,
	POPS,
	CLEARS,
	ADD,
	SUB,
	MUL,
	DIV,
	ADDS,
	SUBS,
	MULS,
	DIVS,
	LT,
	GT,
	EQ,
	LTS,
	GTS,
	EQS,
	AND,
	OR,
	NOT,
	ANDS,
	ORS,
	NOTS,
	INT2FLOAT,
	FLOAT2INT,
	FLOAT2R2EINT,
	FLOAT2R2OINT,
	INT2CHAR,
	STRI2INT,
	READ,
	WRITE,
	CONCAT,
	STRLEN,
	GETCHAR,
	SETCHAR,
	TYPE,
	LABEL,
	JUMP,
	JUMPIFEQ,
	JUMPIFNEQ,
	JUMPIFEQS,
	JUMPIFNEQS,
	BREAK,
	DPRINT,
	};

    int parse(FILE *input, FILE *output);



#endif