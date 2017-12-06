#
#    + ------------- +
#    | PROJEKT IFJ17 |
#    + ------------- +
#
#    Společný projekt IFJ a IAL 2017
#        Implementace prekladace imperativního jazyka IFJ17.
#
#    Varianta zadanie:
#        Tým 025, varianta I
#
#    Soubor:
#        Makefile
#
#    Autori:
#        xmarko15 Peter Marko
#        xmechl00 Stanislav Mechl
#        xnanoa00 Andrej Nano
#        xsvand06 Švanda Jan
#

##   GENERAL MAKEFILE

CC=gcc
CFLAGS=-O2 -std=c99 -g -Wall -pedantic
OBJFILES=$(patsubst src/%.c, build/%.o, $(shell ls src/*.c))
OUTFILE=ifj2017
ZIPNAME=xnanoa00

all: build

.PHONY: clean run zip test

build/%.o: src/%.c src/*.h
	$(CC) $(CFLAGS) -c $< -o $@

build: $(OBJFILES)
	$(CC) $(CFLAGS) $^ -o $(OUTFILE)

clean:
	rm -rf build/* $(OUTFILE) $(ZIPNAME).zip

run:
	./$(OUTFILE)
	
test:
	python3 tests/test.py
	
zip:
	cp Makefile-zip src/Makefile
	cp rozdeleni src/rozdeleni
	cp rozsireni src/rozsireni
	cp dokumentace.pdf src/dokumentace.pdf
	cd src; zip -r ../$(ZIPNAME).zip *
	cd src; rm Makefile rozdeleni rozsireni *.pdf
