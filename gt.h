#ifndef GT_H
#define GT_H
/*
   gt.h

   various global stuff.
*/

#include <libtcod.h>
//#include "objects.h"
//#include "creature.h"

#define GT_VERSION  "0.0.14"

void die(char *message);
int  dice(int num, int sides, signed int modifier);
void mess(char *message);
void mess_color(TCOD_color_t color, char *message);
int perc(int n, int p);
char askplayer(char *question);

extern TCOD_random_t rndgen;
#define ri(min, max) TCOD_random_get_int(rndgen, min, max)

#define MAX_MESS 22
#endif
