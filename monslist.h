#ifndef _MONSLIST_H
#define _MONSLIST_H

#include "creature.h"

START_MONSTERS
/*       name,           char, level, hp, ai,                     speed, str, phys, intl, know, dex, cha, thac0 */ 
MONSTER("kitty cat",      'f',   1,    9, simpleoutdoorpathfinder, 0.6,   3,   9,    9,    4,    18,  18,  3)
MONSTER("grinning idiot", 'g',   1,    6, defaultai,               0.3,   4,   4,    1,    1,     4,   1,  2)
MONSTER("orc",            'o',   1,    8, hostileai,               0.9,   12, 11,    8,    8,     7,   5,  6)
MONSTER("lava amoeba",    'a',   2,   12, defaultai,               0.1,   2,   3,    1,    1,     2,   1,  1)
MONSTER("speed freak",    's',   2,   10, defaultai,               2.4,   12,  7,    4,    9,     9,   2,  5)
MONSTER("silent creeper", 'c',   3,   19, hostileai,               0.7,   10, 12,   15,   10,    17,  11,  8)
MONSTER("psycho killer",  'k',   4,   27, simpleoutdoorpathfinder, 1.3,   14, 13,   16,   10,     9,  10, 10)
MONSTER("zombie geesus",  'j', 100, 1000, simpleoutdoorpathfinder, 1.0,   18, 10,    2,    2,     8,   2, 15)

END_MONSTERS

#define NUM_MONSTERS 6

#endif
