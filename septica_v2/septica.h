#ifndef SEPTICA_H
#define SEPTICA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_CARTI 52
#define CARTI_INITIALE 5

// structura pentru o carte
typedef struct {
    int valoare;  // 1=As, 2-10, 11=Valet, 12=Dama, 13=Rege
    int culoare;  // 0=inima, 1=caro, 2=trefla, 3=pica
} Carte;

// structura pentru un jucator
typedef struct {
    char nume[50];
    Carte mana[MAX_CARTI];
    int nr_carti;
    int scor;
} Jucator;

// pachetul de carti
typedef struct {
    Carte carti[MAX_CARTI];
    int top;
} Pachet;

// prototipuri functii
void initPachet(Pachet *p);
void amestecaPachet(Pachet *p);
void distribuieCarti(Pachet *p, Jucator *j1, Jucator *j2, Carte *pemasa);
void afiseazaMana(Jucator *j);
int cartePotrivita(Carte dejos, Carte demana, int carti_de_luat);
void jucatorJoaca(Pachet *p, Jucator *j, Jucator *adv, Carte *pemasa, int *carti_de_luat);
void calculatorJoaca(Pachet *p, Jucator *calc, Jucator *uman, Carte *pemasa, int *carti_de_luat);
int cineastigat(Jucator *j);
void trageCarti(Pachet *p, Jucator *j, int cate);
char* numeValoare(int v);
char* numeCuloare(int c);

#endif
