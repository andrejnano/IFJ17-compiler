/*
IFJ17 Projekt
main.c
*/

#include <stdio.h>

/* Jednoduchy vypis na debugovanie */
#define LOG(x) printf(x) && printf("\n");

int main(int argc, char* argv[])
{
    LOG("Funguje to.");
    return 0;
}
