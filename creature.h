#ifndef CREATURE_H
#define CREATURE_H

/*
 * stuff for dealing with monsters and other creatures
 */

typedef struct {
        signed char str;
        signed char phys;
        signed char intl;
        signed char know;
        signed char dex;
        signed char cha;
} sattr_t;

typedef struct {
        unsigned char str;
        unsigned char phys;
        unsigned char intl;
        unsigned char know;
        unsigned char dex;
        unsigned char cha;
} uattr_t;

typedef struct {
        struct object *head;
        struct object *body;
        struct object *gloves;
        struct object *footwear;
        struct object *robe;
        struct object *amulet;
        struct object *ring[10];
} wear_t;

struct creature {
	short x, y;
        short hp, maxhp;
        int xp;
        short ac;
        uattr_t attr;
        short level;
        short race, cla;
        char wvfactor;
        short worldview;
        struct object *inventory;
        struct object *weapon;
        wear_t w;
        long flags;
        char name[30];
        int c;             // character, for monsters.
        float speed;
        float movement;
        TCOD_color_t color;
        void (*ai)(struct creature *, struct creature *);      // artificial intelligence handler!!
        int goalx, goaly;                   // for simple outdoor pathfinder ai
        TCOD_map_t fov;
        struct creature *prev;
        struct creature *next;
        struct creature *attacker;
        short thac0;
        float skill[10];
};

typedef struct creature creature_t;
typedef struct creature player_t;

/* for rings */
#define  LEFTPINKY 0
#define  LEFTRINGF 1
#define  LEFTMIDDL 2
#define  LEFTINDEX 3
#define  LEFTTHUMB 4
#define RIGHTTHUMB 5
#define RIGHTINDEX 6
#define RIGHTMIDDL 7
#define RIGHTRINGF 8
#define RIGHTPINKY 9

/* skills */
#define SK_NOSKILL   0
#define SK_BAREHANDS 1


#define START_MONSTERS creature_t monsters[] = {
#define MONSTER(name, character, level, hp, ai, speed, str, phys, intl, know, dex, cha, tohit) { 0, 0, hp, hp, 0, 0, { str, phys, intl, know, dex, cha }, level, 0, 0, 0, 0, 0, 0, { 0,0,0,0,0,0, {0,0,0,0,0,0,0,0,0,0} }, 0, name, character, speed, 0, {0,0,0}, ai, 0, 0, 0, 0, 0, 0, tohit, {0,0,0,0,0,0,0,0,0,0}},
#define END_MONSTERS };


#define RACE_HUMAN 1
#define RACE_ELF   2
#define RACES 2

#define CLASS_FIGHTER 1
#define CLASS_WIZARD  2
#define CLASSES 2


typedef struct {
        char name[25];
        signed int hp_mod;
        signed int phys_mod;
        signed int str_mod;
        signed int intl_mod;
        signed int know_mod;
        signed int dex_mod;
        signed int cha_mod;
        short minphys;
        short minstr;
        short minintl;
        short minknow;
        short mindex;
        short mincha;
} race_t;

typedef struct {
        char name[25];
        signed int hp_mod;
        signed int phys_mod;
        signed int str_mod;
        signed int intl_mod;
        signed int know_mod;
        signed int dex_mod;
        signed int cha_mod;
        short minphys;
        short minstr;
        short minintl;
        short minknow;
        short mindex;
        short mincha;
} class_t;


#define pstr  player->attr.str
#define pintl player->attr.intl
#define pknow player->attr.know
#define pphys player->attr.phys
#define pdex  player->attr.dex
#define pcha  player->attr.cha

#define is_wizard (player->cla == CLASS_WIZARD)
#define is_fighter (player->cla == CLASS_FIGHTER)

/* Prototypes */

void init_player(player_t *player, int x, int y);
void recalculate_worldview(player_t *player);

/* AI */
void defaultai(creature_t *creature, creature_t *player);
void simpleoutdoorpathfinder(creature_t *creature, creature_t *player);
void hostileai(creature_t *creature, creature_t *player);


/* Monster specifics */
void init_monsters(world_t *world, player_t *player);
void move_monsters(world_t *world, creature_t *player);

#endif
