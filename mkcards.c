#include <stdio.h>
#include "cards.h"

card_t deck[7][3][10];

int main()
{
        int p, c, a;

        for(p=0;p<7;p++) {
                for(c=0;c<3;c++) {
                        for(a=0;a<10;a++) {
                                deck[p][c][a].planet = p;
                                deck[p][c][a].color = c;
                                deck[p][c][a].aspect = a;
                                printf("%s %s card of the %s\n", ccolors[deck[p][c][a].color], planets[deck[p][c][a].planet], aspects[deck[p][c][a].aspect]);
                        }
                }
        }

        return 0;
}

