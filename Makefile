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

##   GENERAL MAKEFILE

CC=gcc
CFLAGS=-DDEBUG -O2 -std=c99 -g -Wall -pedantic
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
	cp -R tests/ src/tests/
	cd src; zip -r ../$(ZIPNAME).zip *
	rm src/Makefile src/rozdeleni
	rm -rf src/tests
