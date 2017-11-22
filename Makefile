#
#    + -------------- +
#    | PROJEKT IFJ17  |
#    + -------------- +
#    Implementace prekladace imperativního jazyka IFJ17.
#
#    Autori:
#        xmarko15 Peter Marko
#        xmechl00 Stanislav Mechl
#        xnanoa00 Andrej Nano
#        xsvand06 Švanda Jan
#
#    Zadanie:
#        Vytvorte program v jazyce C, který nacte zdrojový kód zapsaný ve zdrojovém
#        jazyce IFJ17 a preloží jej do cílového jazyka IFJcode17 (mezikód).
#

##	GENERAL MAKEFILE

# @TODO zip, documentation, tests

CC=gcc
CFLAGS=-DDEBUG -O2 -std=c99 -g -Wall -pedantic
OBJFILES=$(patsubst src/%.c, build/%.o, $(shell ls src/*.c))

all: ifj2017

ifj2017: $(OBJFILES)
	$(CC) $(CFLAGS) build/* -o ifj2017

build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $(subst .c,.o, $(subst src/, build/, $<))

clean:
	rm -rf build/* ifj2017
run:
	build/main