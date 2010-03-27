/*
 *
 * Name Generator!
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "gt.h"
#include "namegen.h"

char *city_initial[] = {
        "An",
        "Am",
        "Ga",
        "Be",
        "Li",
        "Ka",
        "Wu",
        "Yi",
        "Mo",
        "La",
        "De",
        "Al",
        "Fi"
};

char *city_middle1[] = {
        "gar",
        "hin",
        "ma",
        "man",
        "ra",
        "ri",
        "re",
        "le",
        "to",
        "ka",
        "du",
        "ta",
        "si"
};

char *city_middle2[] = {
        "al",
        "e",
        "nin",
        "mme",
        "sa",
        "la",
        "ika",
        "et",
        "ken",
        "ma",
        "i",
        "ke",
        "za"
};

char *city_final[] = {
        "ta",
        "nen",
        "lia",
        "ata",
        "met",
        "wil",
        "st",
        "ria",
        "rin",
        "zi",
        "heim",
        "burg",
        "sa"
};

#define FOREST_SYLLABLES 10

char *forest_initial[] = {
        "Sta",
        "My",
        "Mu",
        "Ga",
        "Ko",
        "Zu",
        "Uba",
        "Ha",
        "Lu",
        "Be"
};

char *forest_middle1[] = {
        "rk",
        "lm",
        "ll",
        "gg",
        "ch",
        "mm",
        "kk",
        "w",
        "x",
        "q"
};

char *forest_middle2[] = {
        "ar",
        "ur",
        "ym",
        "ing",
        "ir",
        "en",
        "al",
        "el",
        "yl",
        "a"
};

char *forest_final[] = {
        "dale",
        "wood",
        "li",
        "alia",
        "est",
        "ly",
        "wind",
        "dwell",
        "vale",
        "rose"
};



void generate_person_name(char *n)
{
        n = NULL;
        return;
}

void generate_city_name(char *n)
{
        int ninit, nm1, nm2, nfinal;
        int num, i;

        /* Choose one initial syllable */
        ninit = ri(0,12);
        strcat(n, city_initial[ninit]);

        /* choose 1-3 middle1 syllables */
        num = ri(0,2);
        for(i=0;i<num;i++) {
                nm1 = ri(0,12);
                strcat(n, city_middle1[nm1]);
        }

        /* 50 % chance of a middle2 syllable */
        num = ri(1,100);
        if(num > 50) {
                nm2 = ri(0,12);
                strcat(n, city_middle2[nm2]);
        }

        /* Choose one final syllable */
        nfinal = ri(0,12);
        strcat(n, city_final[nfinal]);
}

void generate_village_name(char *n)
{
        n = NULL;
        return;
}

void generate_forest_name(char *n)
{
        int ninit, nm1, nm2, nfinal, num;

        /* Choose one initial syllable */
        ninit = ri(0,FOREST_SYLLABLES-1);
        strcat(n, forest_initial[ninit]);

        /* choose 1 middle1 syllables */
        nm1 = ri(0,FOREST_SYLLABLES-1);
        strcat(n, forest_middle1[nm1]);

        /* Choose 1-2 middle2 syllable (1 guaranteed, 50 % chance of 1 more) */
        num = ri(0,FOREST_SYLLABLES-1);
        strcat(n, forest_middle2[num]);

        num = ri(0, FOREST_SYLLABLES-1);
        if(num > 50) {
                nm2 = ri(0, FOREST_SYLLABLES-1);
                strcat(n, forest_middle2[nm2]);
        }

        /* Choose one final syllable */
        nfinal = ri(0, FOREST_SYLLABLES-1);
        strcat(n, forest_final[nfinal]);
}

void generate_name(int type, char *thename)
{
        switch(type) {
                case NAME_PERSON:
                    generate_person_name(thename);
                    break;
                case NAME_CITY:
                    generate_city_name(thename);
                    break;
                case NAME_VILLAGE:
                    generate_village_name(thename);
                    break;
                case NAME_FOREST:
                    generate_forest_name(thename);
                    break;
                default:
                    return;
        }
}
