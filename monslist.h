#ifndef _MONSLIST_H
#define _MONSLIST_H

#include "creature.h"

START_MONSTERS
/*       name,           char, level, hp, ai,                     speed, str, phys, intl, know, dex, cha, thac0, flags */ 
MONSTER("kitty cat",      'f',   1,    9, simpleoutdoorpathfinder, 0.6,   3,   9,    9,    4,    18,  18,  3,     0)   /* ide: MF_CANRESURRECT x9 */
MONSTER("grinning idiot", 'g',   1,    6, defaultai,               0.3,   4,   4,    1,    1,     4,   1,  2,    MF_CANUSEWEAPON|MF_CANHAVEGOLD)
MONSTER("orc",            'o',   1,    8, hostileai,               0.9,   12, 11,    8,    8,     7,   5,  6,    MF_CANUSESIMPLESWORD|MF_CANUSEARMOR|MF_CANHAVEGOLD)
MONSTER("rat",            'r',   1,    5, hostileai,               0.5,   4,   5,    2,    1,     9,   1,  4,     0)
MONSTER("wannabe demon",  '&',   1,    7, hostileai,               0.8,   10,  8,    6,    6,     9,   8,  5,    MF_CANUSEWEAPON|MF_CANUSEARMOR)
MONSTER("giant beetle",   'b',   1,    8, hostileai,               0.4,   10, 12,    6,    3,     8,   2,  5,     0)
MONSTER("baby ghost",     'O',   1,   10, defaultai,               1.2,    7,  1,    9,    8,    17,  15,  7,     0)  /* add special effect/attack! */
MONSTER("young ghost",    'O',   2,   17, defaultai,               1.4,   10,  2,   10,    9,    17,  15,  8,     0)
MONSTER("goblin",         'G',   2,   13, hostileai,               1.0,   13, 12,   10,   10,     9,   7,  7,    MF_CANUSESIMPLESWORD|MF_CANUSEARMOR|MF_CANHAVEGOLD)
MONSTER("lava amoeba",    'a',   2,   12, defaultai,               0.1,   2,   3,    1,    1,     2,   1,  1,     0)
MONSTER("speed freak",    's',   2,   10, defaultai,               2.4,   12,  7,    4,    9,     9,   2,  5,     0)
MONSTER("grown up ghost", 'O',   3,   25, hostileai,               1.5,   12,  3,   12,    9,    17,  15,  9,     0)
MONSTER("silent creeper", 'c',   3,   19, hostileai,               0.7,   10, 12,   15,   10,    17,  11,  8,    MF_CANUSEWEAPON|MF_CANUSEARMOR)
MONSTER("poltergeist",    'O',   4,   27, hostileai,               1.0,   18,  2,    5,    5,    15,   6, 10,     0)
MONSTER("psycho killer",  'k',   4,   27, simpleoutdoorpathfinder, 1.3,   14, 13,   16,   10,     9,  10, 10,    MF_CANUSEWEAPON|MF_CANUSEARMOR)
MONSTER("zombie geesus",  'j', 100, 1000, simpleoutdoorpathfinder, 1.0,   18, 10,    2,    2,     8,   2, 15,    MF_CANUSEWEAPON|MF_CANUSEARMOR)

END_MONSTERS

#define NUM_MONSTERS 10

#endif
