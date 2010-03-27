#include <stdio.h>
#include <string.h>

#include "gt.h"
#include "cards.h"

char *planets[] = { "Mercury", "Venus", "Mars", "Jupiter", "Saturn", "Neptune", "Uranus" };
char *ccolors[] = { "Golden", "Black", "Purple" };
char *aspects[] = { "Soul", "Blood", "Heavens", "Flesh", "Crescent", "Master", "Reaper", "Star", "Fool", "Tiger" };
card_t deck[7][3][10];

void generate_deck()
{
        int p, c, a;

        for(p=0;p<7;p++) {
                for(c=0;c<3;c++) {
                        for(a=0;a<10;a++) {
                                deck[p][c][a].planet = p;
                                deck[p][c][a].color = c;
                                deck[p][c][a].aspect = a;
                                sprintf(deck[p][c][a].name, "%s %s card of the %s\n", ccolors[deck[p][c][a].color], planets[deck[p][c][a].planet], aspects[deck[p][c][a].aspect]);
                        }
                }
        }
}
