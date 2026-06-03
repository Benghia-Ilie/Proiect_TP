#include "septica.h"

void afiseazaTitlu() {
    printf("\n");
    printf("  ==============================\n");
    printf("        JOC DE CARTI - SEPTICA   \n");
    printf("  ==============================\n\n");
}

void jocSinglePlayer() {
    Pachet pachet;
    Jucator uman, calculator;
    Carte pemasa;
    int carti_de_luat = 0;

    // citim numele jucatorului
    printf("\n  Introdu numele tau: ");
    char buf[50];
    if (fgets(buf, sizeof(buf), stdin)) {
        buf[strcspn(buf, "\n")] = '\0';
        strncpy(uman.nume, buf, 49);
    }
    if (strlen(uman.nume) == 0) strcpy(uman.nume, "Jucator");
    strcpy(calculator.nume, "Calculator");
    uman.scor = 0;
    calculator.scor = 0;

    char replay;
    do {
        // initializare joc nou
        carti_de_luat = 0;
        initPachet(&pachet);
        amestecaPachet(&pachet);
        distribuieCarti(&pachet, &uman, &calculator, &pemasa);

        printf("\n  Cartile au fost impartite. Sa incepem!\n");
        printf("  Prima carta pe masa: %-5s de %s\n\n",
            numeValoare(pemasa.valoare), numeCuloare(pemasa.culoare));

        // jocul propriu-zis
        int tur = 0;  // 0 = uman, 1 = calculator
        while (1) {
            if (tur == 0) {
                // randul jucatorului uman
                printf("\n  --- Randul tau (%s) ---\n", uman.nume);
                jucatorJoaca(&pachet, &uman, &calculator, &pemasa, &carti_de_luat);

                if (cineastigat(&uman)) {
                    printf("\n  Felicitari %s, ai castigat!\n", uman.nume);
                    uman.scor += calculator.nr_carti;
                    break;
                }
            } else {
                // randul calculatorului
                calculatorJoaca(&pachet, &calculator, &uman, &pemasa, &carti_de_luat);

                if (cineastigat(&calculator)) {
                    printf("\n  Calculatorul a castigat! Mai incearca.\n");
                    calculator.scor += uman.nr_carti;
                    break;
                }
            }

            // verifica daca pachetul s-a terminat
            if (pachet.top >= MAX_CARTI) {
                printf("\n  Pachetul s-a terminat! Joc fara castigator.\n");
                break;
            }

            tur = 1 - tur;  // schimba randul
        }

        printf("\n  Scor: %s = %d  |  Calculator = %d\n",
            uman.nume, uman.scor, calculator.scor);

        printf("\n  Vrei sa mai joci? (d/n): ");
        char linie[5];
        replay = 'n';
        if (fgets(linie, sizeof(linie), stdin)) replay = linie[0];

    } while (replay == 'd' || replay == 'D');
}

void jocMultiplayerLocal() {
    Pachet pachet;
    Jucator j1, j2;
    Carte pemasa;
    int carti_de_luat = 0;

    printf("\n  Multiplayer - doi jucatori pe acelasi calculator\n");

    printf("  Numele jucatorului 1: ");
    char buf[50];
    if (fgets(buf, sizeof(buf), stdin)) {
        buf[strcspn(buf, "\n")] = '\0';
        strncpy(j1.nume, buf, 49);
    }
    if (strlen(j1.nume) == 0) strcpy(j1.nume, "Jucator 1");

    printf("  Numele jucatorului 2: ");
    if (fgets(buf, sizeof(buf), stdin)) {
        buf[strcspn(buf, "\n")] = '\0';
        strncpy(j2.nume, buf, 49);
    }
    if (strlen(j2.nume) == 0) strcpy(j2.nume, "Jucator 2");

    j1.scor = 0;
    j2.scor = 0;

    char replay;
    do {
        carti_de_luat = 0;
        initPachet(&pachet);
        amestecaPachet(&pachet);
        distribuieCarti(&pachet, &j1, &j2, &pemasa);

        printf("\n  Prima carta pe masa: %-5s de %s\n",
            numeValoare(pemasa.valoare), numeCuloare(pemasa.culoare));

        int tur = 0;
        while (1) {
            Jucator *curent = (tur == 0) ? &j1 : &j2;
            Jucator *advers = (tur == 0) ? &j2 : &j1;

            printf("\n  --- Randul lui %s ---\n", curent->nume);
            printf("  (Apasa Enter cand esti gata)\n");
            // mica pauza ca adversarul sa nu vada cartile
            char tmp[5];
            fgets(tmp, sizeof(tmp), stdin);

            // curata ecranul (compatibil macOS/Linux)
            system("clear");

            printf("  --- Randul lui %s ---\n", curent->nume);
            jucatorJoaca(&pachet, curent, advers, &pemasa, &carti_de_luat);

            if (cineastigat(curent)) {
                printf("\n  %s a castigat runda!\n", curent->nume);
                curent->scor += advers->nr_carti;
                break;
            }

            if (pachet.top >= MAX_CARTI) {
                printf("\n  Pachetul s-a terminat!\n");
                break;
            }

            tur = 1 - tur;
        }

        printf("\n  Scor: %s = %d  |  %s = %d\n",
            j1.nume, j1.scor, j2.nume, j2.scor);

        printf("\n  Inca o runda? (d/n): ");
        char linie[5];
        replay = 'n';
        if (fgets(linie, sizeof(linie), stdin)) replay = linie[0];

    } while (replay == 'd' || replay == 'D');
}

void afiseazaReguli() {
    printf("\n");
    printf("  === REGULILE JOCULUI SEPTICA ===\n\n");
    printf("  Fiecare jucator primeste 5 carti.\n");
    printf("  Pe masa se pune o carta de referinta.\n\n");
    printf("  La fiecare tur poti:\n");
    printf("    - Juca o carta de aceeasi culoare sau valoare\n");
    printf("    - Juca un As (merge pe orice)\n");
    printf("    - Trage o carta daca nu ai nimic potrivit\n\n");
    printf("  Carti speciale:\n");
    printf("    7 si 8 - adversarul trebuie sa ia 2 carti\n");
    printf("             (sau sa joace alt 7/8 - se cumuleaza!)\n\n");
    printf("  Castiga cine ramane primul fara carti.\n");
    printf("  Scorul = numarul de carti ramase la adversar.\n\n");
}

int main() {
    afiseazaTitlu();

    int optiune;
    char linie[10];

    do {
        printf("  Meniu:\n");
        printf("    1. Single-player (vs calculator)\n");
        printf("    2. Multiplayer local (2 jucatori)\n");
        printf("    3. Reguli\n");
        printf("    0. Iesire\n");
        printf("\n  Alege: ");

        optiune = -1;
        if (fgets(linie, sizeof(linie), stdin)) {
            sscanf(linie, "%d", &optiune);
        }

        switch (optiune) {
            case 1:
                jocSinglePlayer();
                break;
            case 2:
                jocMultiplayerLocal();
                break;
            case 3:
                afiseazaReguli();
                break;
            case 0:
                printf("\n  La revedere!\n\n");
                break;
            default:
                printf("  Optiune invalida.\n\n");
        }
    } while (optiune != 0);

    return 0;
}
