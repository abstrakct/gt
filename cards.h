#ifndef CARDS_H
#define CARDS_H

/* 
 * Stuff for dealing with The Cards
 */


typedef struct {
        short planet;
        short color;
        short aspect;
        char name[40];
} card_t;

extern card_t deck[7][3][10];

void generate_deck();

#define MERCURY 0
#define VENUS 1
#define MARS 2
#define JUPITER 3
#define SATURN 4
#define NEPTUNE 5
#define URANUS 6

#define C_GOLD 0
#define C_BLACK 1
#define C_PURPLE 2

#define A_SOUL 0
#define A_BLOOD 1
#define A_HEAVEN 2
#define A_FLESH 3
#define A_CRESCENT 4
#define A_MASTER 5
#define A_REAPER 6
#define A_STAR 7
#define A_FOOL 8
#define A_TIGEER 9

#endif
