#include "septica.h"

// returneaza numele valorii unei carti
char* numeValoare(int v) {
    switch (v) {
        case 1:  return "As";
        case 11: return "Valet";
        case 12: return "Dama";
        case 13: return "Rege";
        default: {
            // pentru valorile 2-10 construim un string static
            static char buf[3];
            sprintf(buf, "%d", v);
            return buf;
        }
    }
}

// returneaza simbolul culorii
char* numeCuloare(int c) {
    switch (c) {
        case 0: return "Inima ";
        case 1: return "Caro  ";
        case 2: return "Trefla";
        case 3: return "Pica  ";
        default: return "?";
    }
}

// initializeaza pachetul cu toate cele 52 de carti
void initPachet(Pachet *p) {
    int i = 0;
    for (int culoare = 0; culoare < 4; culoare++) {
        for (int val = 1; val <= 13; val++) {
            p->carti[i].valoare = val;
            p->carti[i].culoare = culoare;
            i++;
        }
    }
    p->top = 0;
}

// amesteca pachetul folosind algoritmul Fisher-Yates
void amestecaPachet(Pachet *p) {
    srand((unsigned int)time(NULL));
    for (int i = MAX_CARTI - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        // swap
        Carte temp = p->carti[i];
        p->carti[i] = p->carti[j];
        p->carti[j] = temp;
    }
}

// da cate 5 carti fiecarui jucator si pune o carte pe masa
void distribuieCarti(Pachet *p, Jucator *j1, Jucator *j2, Carte *pemasa) {
    // initializam mainile goale
    j1->nr_carti = 0;
    j2->nr_carti = 0;

    for (int i = 0; i < CARTI_INITIALE; i++) {
        j1->mana[j1->nr_carti++] = p->carti[p->top++];
        j2->mana[j2->nr_carti++] = p->carti[p->top++];
    }

    // prima carta pe masa (sarim cartile speciale la inceput)
    *pemasa = p->carti[p->top++];
    while (pemasa->valoare == 7 || pemasa->valoare == 8) {
        *pemasa = p->carti[p->top++];
    }
}

// afiseaza cartile din mana unui jucator
void afiseazaMana(Jucator *j) {
    printf("  Cartile lui %s:\n", j->nume);
    for (int i = 0; i < j->nr_carti; i++) {
        printf("    [%d] %-5s de %s\n",
            i + 1,
            numeValoare(j->mana[i].valoare),
            numeCuloare(j->mana[i].culoare));
    }
}

// verifica daca o carte din mana poate fi jucata pe carta de pe masa
// daca sunt carti de luat acumulate, poti juca doar 7 sau 8
int cartePotrivita(Carte dejos, Carte demana, int carti_de_luat) {
    if (carti_de_luat > 0) {
        return (demana.valoare == 7 || demana.valoare == 8);
    }
    // aceeasi culoare sau aceeasi valoare sau As
    if (demana.culoare == dejos.culoare) return 1;
    if (demana.valoare == dejos.valoare) return 1;
    if (demana.valoare == 1) return 1;  // Asul merge mereu
    return 0;
}

// trage un numar de carti din pachet in mana jucatorului
void trageCarti(Pachet *p, Jucator *j, int cate) {
    for (int i = 0; i < cate; i++) {
        if (p->top >= MAX_CARTI) {
            printf("  (Pachetul s-a terminat!)\n");
            break;
        }
        j->mana[j->nr_carti++] = p->carti[p->top++];
    }
}

// scoate o carta din mana jucatorului (shifteaza restul)
static void scoateCarta(Jucator *j, int index) {
    for (int i = index; i < j->nr_carti - 1; i++) {
        j->mana[i] = j->mana[i + 1];
    }
    j->nr_carti--;
}

// gestioneaza turul jucatorului uman
void jucatorJoaca(Pachet *p, Jucator *j, Jucator *adv, Carte *pemasa, int *carti_de_luat) {
    (void)adv;
    printf("\n  Carta de pe masa: %-5s de %s\n",
        numeValoare(pemasa->valoare), numeCuloare(pemasa->culoare));

    if (*carti_de_luat > 0) {
        printf("  !! Trebuie sa iei %d carti sau sa joci un 7/8 !!\n", *carti_de_luat);
    }

    afiseazaMana(j);
    printf("  [0] Trage o carte din pachet\n");

    // citim optiunea
    int ales = -1;
    char linie[20];
    while (1) {
        printf("  Alegerea ta: ");
        if (fgets(linie, sizeof(linie), stdin) == NULL) continue;
        if (sscanf(linie, "%d", &ales) != 1) continue;

        if (ales == 0) {
            // trage carte(le)
            if (*carti_de_luat > 0) {
                trageCarti(p, j, *carti_de_luat);
                printf("  Ai luat %d carti.\n", *carti_de_luat);
                *carti_de_luat = 0;
            } else {
                trageCarti(p, j, 1);
                printf("  Ai tras o carte.\n");
            }
            return;
        }

        if (ales < 1 || ales > j->nr_carti) {
            printf("  Optiune invalida, incearca din nou.\n");
            continue;
        }

        Carte aleasa = j->mana[ales - 1];
        if (!cartePotrivita(*pemasa, aleasa, *carti_de_luat)) {
            printf("  Nu poti juca acea carte! Trebuie sa fie aceeasi culoare, valoare sau un As.\n");
            continue;
        }

        // joaca carta
        printf("  Ai jucat: %-5s de %s\n",
            numeValoare(aleasa.valoare), numeCuloare(aleasa.culoare));
        *pemasa = aleasa;
        scoateCarta(j, ales - 1);

        if (aleasa.valoare == 7 || aleasa.valoare == 8) {
            *carti_de_luat += 2;
            printf("  Adversarul trebuie sa ia %d carti!\n", *carti_de_luat);
        } else {
            *carti_de_luat = 0;
        }
        return;
    }
}

// calculatorul alege automat cea mai buna carta
void calculatorJoaca(Pachet *p, Jucator *calc, Jucator *uman, Carte *pemasa, int *carti_de_luat) {
    (void)uman;
    printf("\n  Randul calculatorului...\n");

    // daca trebuie sa ia carti, incearca sa joace 7 sau 8
    if (*carti_de_luat > 0) {
        for (int i = 0; i < calc->nr_carti; i++) {
            if (calc->mana[i].valoare == 7 || calc->mana[i].valoare == 8) {
                printf("  Calculatorul joaca: %-5s de %s\n",
                    numeValoare(calc->mana[i].valoare),
                    numeCuloare(calc->mana[i].culoare));
                *pemasa = calc->mana[i];
                *carti_de_luat += 2;
                scoateCarta(calc, i);
                printf("  Acum tu trebuie sa iei %d carti!\n", *carti_de_luat);
                return;
            }
        }
        // nu are, ia cartile
        printf("  Calculatorul ia %d carti.\n", *carti_de_luat);
        trageCarti(p, calc, *carti_de_luat);
        *carti_de_luat = 0;
        return;
    }

    // cauta cea mai buna carta: prioritate 7 > 8 > aceeasi valoare > aceeasi culoare
    int best = -1;
    int best_prio = -1;
    for (int i = 0; i < calc->nr_carti; i++) {
        if (!cartePotrivita(*pemasa, calc->mana[i], 0)) continue;
        int prio = 0;
        if (calc->mana[i].valoare == 7) prio = 4;
        else if (calc->mana[i].valoare == 8) prio = 3;
        else if (calc->mana[i].valoare == pemasa->valoare) prio = 2;
        else prio = 1;
        if (prio > best_prio) { best_prio = prio; best = i; }
    }

    if (best >= 0) {
        printf("  Calculatorul joaca: %-5s de %s\n",
            numeValoare(calc->mana[best].valoare),
            numeCuloare(calc->mana[best].culoare));
        if (calc->mana[best].valoare == 7 || calc->mana[best].valoare == 8) {
            *carti_de_luat += 2;
            printf("  Trebuie sa iei %d carti!\n", *carti_de_luat);
        } else {
            *carti_de_luat = 0;
        }
        *pemasa = calc->mana[best];
        scoateCarta(calc, best);
    } else {
        // nu are nicio carta buna, trage din pachet
        printf("  Calculatorul trage o carte.\n");
        trageCarti(p, calc, 1);
    }
}

// verifica daca jucatorul a ramas fara carti
int cineastigat(Jucator *j) {
    return j->nr_carti == 0;
}
