#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define VSTUPNI_SOUBOR "cyklistika.txt"
#define VYSTUPNI_SOUBOR "vysledkova_listina.txt"
#define POCET_RADKU 150
#define POCET_ZNAKU 100
#define DEL ";:\n"

typedef struct {
    int h;
    int m;
    float s;
} CAS;

typedef struct {
    int start_cislo;
    char prijmeni[20];
    char jmeno[20];
    char narodnost[5];
    int rocnik;
    char klub[50];
    CAS cas;
} ZAVODNIK;

int nacti_soubor(ZAVODNIK *zavodnik) {
    int index_zavodnika = 0;
    FILE *soubor = fopen(VSTUPNI_SOUBOR, "r");
    if (soubor == NULL) {
        printf("Soubor %s se nepodarilo otevrit.\n", VSTUPNI_SOUBOR);
        return 0;
    } else {
        char radek[POCET_ZNAKU];
        char *cast_radku;
        while (fgets(radek, sizeof(radek), soubor) != NULL) {
            if (index_zavodnika != 0) {
                int sloupec = 0;
                cast_radku = strtok(radek, DEL);
                while (cast_radku != NULL) {
                    switch (sloupec) {
                        case 0:
                            zavodnik[index_zavodnika - 1].start_cislo = atoi(cast_radku);
                            break;
                        case 1:
                            strcpy(zavodnik[index_zavodnika - 1].prijmeni, cast_radku);
                            break;
                        case 2:
                            strcpy(zavodnik[index_zavodnika - 1].jmeno, cast_radku);
                            break;
                        case 3:
                            strcpy(zavodnik[index_zavodnika - 1].narodnost, cast_radku);
                            break;
                        case 4:
                            zavodnik[index_zavodnika - 1].rocnik = atoi(cast_radku);
                            break;
                        case 5:
                            strcpy(zavodnik[index_zavodnika - 1].klub, cast_radku);
                            break;
                        case 6:
                            zavodnik[index_zavodnika - 1].cas.h = atoi(cast_radku);
                            break;
                        case 7:
                            zavodnik[index_zavodnika - 1].cas.m = atoi(cast_radku);
                            break;
                        case 8:
                            zavodnik[index_zavodnika - 1].cas.s = atof(cast_radku);
                            break;
                    }
                    cast_radku = strtok(NULL, DEL);
                    sloupec++;
                }
            }
            index_zavodnika++;
        }
        if (fclose(soubor) == EOF) {
            printf("Soubor %s se nepodarilo zavrit.\n", VSTUPNI_SOUBOR);
        }
    }
    return index_zavodnika - 1;
}

int rok() {
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    return timeinfo->tm_year + 1900;
}

float prevod_casu_na_sekundy(CAS cas) {
    return (float) cas.h * 3600 + (float) cas.m * 60 + cas.s;
}

CAS sekundy_na_cas(float celkove_sekundy) {
    CAS vysledny_cas;
    vysledny_cas.h = (int) celkove_sekundy / 3600;
    vysledny_cas.m = ((int) celkove_sekundy % 3600) / 60;
    vysledny_cas.s = celkove_sekundy - (vysledny_cas.h * 3600 + vysledny_cas.m * 60);
    return vysledny_cas;
}

void serad_zavodniky(ZAVODNIK *zavodnik, int pocet) {
    for (int i = 0; i < pocet - 1; ++i) {
        for (int j = 0; j < pocet - i - 1; ++j) {
            int cas1 = prevod_casu_na_sekundy(zavodnik[j].cas);
            int cas2 = prevod_casu_na_sekundy(zavodnik[j + 1].cas);
            if (cas1 > cas2) {
                ZAVODNIK temp = zavodnik[j];
                zavodnik[j] = zavodnik[j + 1];
                zavodnik[j + 1] = temp;
            }
        }
    }
}

void vypis(ZAVODNIK *zavodnik, int pocet) {
    printf("startovni cislo   prijmeni    jmeno    narodnost     rocnik       klub                   cas");
    for (int i = 0; i < pocet; ++i) {
        printf("%10d %14s %14s %9s %7d %35s %02d:%02d:%04.1f\n",
               zavodnik[i].start_cislo, zavodnik[i].prijmeni, zavodnik[i].jmeno, zavodnik[i].narodnost,
               zavodnik[i].rocnik, zavodnik[i].klub, zavodnik[i].cas.h, zavodnik[i].cas.m, zavodnik[i].cas.s);
    }
    printf("Na startu je %d zavodniku\n", pocet);

    int nejstarsi_zavodnik = 0;
    for (int i = 0; i < pocet; ++i) {
        if (zavodnik[nejstarsi_zavodnik].rocnik > zavodnik[i].rocnik) {
            nejstarsi_zavodnik = i;
        }
    }
    int vek = rok() - zavodnik[nejstarsi_zavodnik].rocnik;

    printf("Nejstarsim zavodnikem je %s %s narozden v roce %d. Je mu %d let.\n",
           zavodnik[nejstarsi_zavodnik].jmeno, zavodnik[nejstarsi_zavodnik].prijmeni,
           zavodnik[nejstarsi_zavodnik].rocnik, vek);

    int cesi = 0;
    int slovaci = 0;
    int jine = 0;
    for (int i = 0; i < pocet; ++i) {
        if (strcmp(zavodnik[i].narodnost, "CZE") == 0) {
            cesi++;
        } else if (strcmp(zavodnik[i].narodnost, "SVK") == 0) {
            slovaci++;
        } else {
            jine++;
        }
    }
    printf("Zavodnici dle narodnsoti.\n Cesi %d. \n Slovaci %d. \n Jine %d. \n", cesi, slovaci, jine);
}

void zapis_do_souboru(ZAVODNIK *zavodnik, int pocet) {
    FILE *soubor = fopen(VYSTUPNI_SOUBOR, "w");
    if (soubor == NULL) {
        printf("Soubor %s se nepodarilo otevrit.\n");
        return;
    }
    fprintf(soubor,
            "poradi    startovni cislo   prijmeni    jmeno    narodnost     rocnik       klub                   cas     ztrata\n");

    float vitezny_cas = prevod_casu_na_sekundy(zavodnik[0].cas);

    for (int i = 0; i < pocet; ++i) {
        float cas = prevod_casu_na_sekundy(zavodnik[i].cas);
        float ztrata = cas - vitezny_cas;
        CAS vysledny_cas = sekundy_na_cas(ztrata);
        fprintf(soubor, " %6d %10d %14s %14s %9s %7d %35s %02d:%02d:%04.1f ",
                i + 1, zavodnik[i].start_cislo, zavodnik[i].prijmeni, zavodnik[i].jmeno, zavodnik[i].narodnost,
                zavodnik[i].rocnik, zavodnik[i].klub, zavodnik[i].cas.h, zavodnik[i].cas.m, zavodnik[i].cas.s);
        if (i == 0) {
            fprintf(soubor, "\n");
        } else {
            fprintf(soubor, "     +%02d:%02d:%04.1f\n", vysledny_cas.h, vysledny_cas.m, vysledny_cas.s);
        }
    }
    if (fclose(soubor) == EOF){
        printf("Soubor %s se nepodarilo zavrit.\n", VYSTUPNI_SOUBOR);
    }
}


int main() {
    ZAVODNIK zavodnik[POCET_RADKU];
    int pocet = nacti_soubor(zavodnik);
    vypis(zavodnik, pocet);
    serad_zavodniky(zavodnik, pocet);
    zapis_do_souboru(zavodnik, pocet);
    return 0;
}
