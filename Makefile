#
# Simple Makefile for main.c
#

CC=gcc
CFLAGS=-O2 -std=c99 -g -Wall -pedantic

all: main

main: src/main.c
	$(CC) $(CFLAGS) src/main.c -o build/main

run: 
	build/main