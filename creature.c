/*
 * stuff for dealing with monsters and other creatures.
 */

#include <stdlib.h>
#include <stdio.h>
#include <libtcod.h>

#include "gt.h"
#include "objects.h"
#include "objdefines.h"
#include "world.h"
#include "creature.h"
#include "monslist.h"
#include "you.h"
#include "display.h"
#include "inventory.h"
#include "attack.h"

creature_t *mon;

signed int hp_phys_adj[] = { 0, -3, -2, -2, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 3, 4, 5 };
char *skillstr[] = { "", "Bare hands fighting", "Long sword", "Short sword", "Battle axe" };

race_t races[RACES+1] = {       /* name, hp, phys, str, intl, know, dex, cha, minphys, minstr, minintl, minknow, mindex, mincha*/
        { "", 0,
                 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 0 },
        { "Human", 0,
                 0, 0, 0, 0, 0, 0,
                 8, 8, 8, 8, 8, 8 },
        { "Elf", 2,
                -1, 0, 2, 0, 0, 0,
                 8, 8, 8, 8, 8, 8 }
};

class_t classes[CLASSES+1] = {  /* name, hp, phys, str, intl, know, dex, cha, minphys, minstr, minintl, minknow, mindex, mincha*/ 
        { "", 0,
                0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0 },
        { "Fighter", 0,
                2, 2, 0, 0, 0, 0,
                13, 14, 4, 5, 9, 7},
        { "Wizard", 0, 
                0, 0, 2, 2, 0, 0,
                8, 8, 14, 8, 9, 8  }
};

extern int num_objects;
extern obj_t objects[];

void defaultai(creature_t *creature, creature_t *player)
{
        int dir;

        dir = ri(1,9);
        switch(dir) {
                case 1:
                        creature->x--;
                        creature->y++;
                        break;
                case 2: 
                        creature->y++;
                        break;
                case 3:
                        creature->y++;
                        creature->x++;
                        break;
                case 4:
                        creature->x--;
                        break;
                case 5: 
                        break;
                case 6:
                        creature->x++;
                        break;
                case 7:
                        creature->x--;
                        creature->y--;
                        break;
                case 8:
                        creature->y--;
                        break;
                case 9:
                        creature->x++;
                        creature->y--;
                        break;
        }

        if(creature->x < 0)
                creature->x = 0;
        if(creature->y < 0)
                creature->y = 0;

        if(creature->x >= XSIZE-1)
                creature->x = XSIZE-2;
        if(creature->y >= YSIZE-1)
                creature->y = YSIZE-2;

}

void simpleoutdoorpathfinder(creature_t *creature, creature_t *player)
{
        int choice;

        if(!creature->goalx || !creature->goaly || creature->x == creature->goalx || creature->y == creature->goaly) {
                // basically, if we have no goal, or have reached the goal, set a new goal.
                creature->goalx = ri(1,XSIZE-1);
                creature->goaly = ri(1,YSIZE-1);
        }

        // now, let's try to avoid the stupid diagonal only walk.

        choice = ri(1,100);
        if(choice <= 45) {
                if(creature->x > creature->goalx)
                        creature->x--;
                if(creature->x < creature->goalx)
                        creature->x++;
        } else if(choice > 45 && choice <= 90) {
                if(creature->y > creature->goaly)
                        creature->y--;
                if(creature->y < creature->goaly)
                        creature->y++;
        } else if(choice > 90) {
                // maybe not extremely useful, but adds randomness to the movements,
                // as if the creature's attention was briefly caught by something else..

                switch(choice) {
                        case 91:
                                creature->x--;
                                creature->y++;
                                break;
                        case 92: 
                                creature->y++;
                                break;
                        case 93:
                                creature->y++;
                                creature->x++;
                                break;
                        case 94:
                                creature->x--;
                                break;
                        case 95: 
                                break;
                        case 96:
                                creature->x++;
                                break;
                        case 97:
                                creature->x--;
                                creature->y--;
                                break;
                        case 98:
                                creature->y--;
                                break;
                        case 99:
                                creature->x++;
                                creature->y--;
                                break;
                        case 100:
                                break;
                }
        }
}

int aisnexttob(struct creature *a, struct creature *b)
{
        if(!a)
                return 0;
        if(!b)
                return 0;
                                
        if((a->x == b->x-1 && a->y == b->y) ||
                        (a->x == b->x+1 && a->y == b->y) ||
                        (a->y == b->y-1 && a->x == b->x) ||
                        (a->y == b->y+1 && a->x == b->x) ||
                        (a->x == b->x-1 && a->y == b->y-1) ||
                        (a->x == b->x+1 && a->y == b->y+1) ||
                        (a->x == b->x-1 && a->y == b->y+1) ||
                        (a->x == b->x+1 && a->y == b->y-1))
                return 1;
        else
                return 0;
}

void hostileai(creature_t *creature, creature_t *player)
{
        if(player->x >= (creature->x-10) && player->x <= creature->x+10 && player->y >= creature->y-10 && player->y <= creature->y+10) {
                creature->goalx = player->x;
                creature->goaly = player->y;

                if(creature->x > creature->goalx) {
                        creature->x--;
                        if(creature->y == player->y && creature->x == player->x)
                                creature->x++;
                }

                if(creature->x < creature->goalx) {
                        creature->x++;
                        if(creature->y == player->y && creature->x == player->x)
                                creature->x--;
                }

                if(creature->y > creature->goaly) {
                        creature->y--;
                        if(creature->y == player->y && creature->x == player->x)
                                creature->y++;
                }

                if(creature->y < creature->goaly) {
                        creature->y++;
                        if(creature->y == player->y && creature->x == player->x)
                                creature->y--;
                }

                if(aisnexttob(creature, player)) {
                        creature->attacker = player;
                        player->attacker = creature;
                }
        } else {
                creature->attacker = NULL;
                simpleoutdoorpathfinder(creature, player);
        }
}

void movemonstertowards(struct creature *creature, struct creature *dest)
{
        creature->goalx = dest->x;
        creature->goaly = dest->y;

        if(creature->x > creature->goalx)
                creature->x--;
        if(creature->x < creature->goalx)
                creature->x++;
        if(creature->y > creature->goaly)
                creature->y--;
        if(creature->y < creature->goaly)
                creature->y++;
}



void move_monsters(world_t *world, creature_t *player)
{
        creature_t *tmp;

        tmp = mon->next;
        while(tmp != NULL) {
                if(tmp->attacker) {
                        if(aisnexttob(tmp->attacker, tmp)) {
                                attack(tmp, tmp->attacker, world);
                        } else {
                                tmp->movement += tmp->speed;
                                while(tmp->movement >= 1.0) {
                                        world->cell[tmp->y][tmp->x].monster = NULL;
                                        hostileai(tmp, tmp->attacker);
                                        world->cell[tmp->y][tmp->x].monster = tmp;
                                        tmp->movement--;
                                        if(aisnexttob(tmp->attacker, tmp)) {
                                                attack(tmp, tmp->attacker, world);
                                                return;
                                        }

                                }
                        }
                } else {
                        tmp->movement += tmp->speed;
                        while(tmp->movement >= 1.0) {
                                world->cell[tmp->y][tmp->x].monster = NULL;
                                tmp->ai(tmp, player);
                                world->cell[tmp->y][tmp->x].monster = tmp;
                                tmp->movement--;
                        }
                }
                tmp = tmp->next;
        }
}


void init_monsters(world_t *world, player_t *player)
{
        int i, num, what;
        creature_t *tmp;

        mon = malloc(sizeof(creature_t));
        if(!mon)
                die("memory allocation error!");

        tmp = mon;
        num = ri(XSIZE/8, YSIZE/4);
        printf("\tcreating %d monsters..\n", num);

        for(i=0;i<num;i++) {
                int ok = 0;
                int p;

                while(!ok) {
                        what = ri(0,NUM_MONSTERS-1);
                        p = ri(1,100);
                        if(p <= 80) {
                                if(monsters[what].level == player->level)
                                        ok = 1;
                                else
                                        ok = 0;
                        } else if (p > 80 && p <= 95) {
                                if(monsters[what].level == player->level+1)
                                        ok = 1;
                                else
                                        ok = 0;
                        } else if (p > 95 && p <= 100) {
                                if(monsters[what].level == player->level+2)
                                        ok = 1;
                                else
                                        ok = 0;
                        }
                }

                tmp->next = malloc(sizeof(creature_t));
                if(!tmp->next)
                        die("memory allocation error!");

                *(tmp->next) = monsters[what];
                tmp->next->prev = tmp;
                tmp = tmp->next;
                tmp->x = ri(0,XSIZE-1);
                tmp->y = ri(0,YSIZE-1);
                tmp->next = NULL;
                tmp->attr.str += ri(-1,1);
                tmp->attr.phys += ri(-1,1);
                tmp->attr.intl += ri(-1,1);
                tmp->attr.know += ri(-1,1);
                tmp->attr.dex += ri(-1,1);
                tmp->attr.cha += ri(-1,1);
                tmp->thac0 = (tmp->attr.dex/3) + (tmp->attr.str/4);
                init_monster_inventory(tmp);
                world->cell[tmp->y][tmp->x].monster = tmp;
        }

        return;
}



int get_init_hp(player_t *player)
{
        return dice(1, 10, 10+hp_phys_adj[pphys]);
}

void recalculate_worldview(player_t *player)
{
        int old;
        old = player->worldview;

        if(player->wvfactor <= 45)
                player->worldview = 0;
        if(player->wvfactor > 45 && player->wvfactor <= 70)
                player->worldview = 1;
        if(player->wvfactor > 70)
                player->worldview = 2;

        if(old > player->worldview)
                you_c(TCOD_light_green, "feel more optimistic!");
        if(old < player->worldview)
                you_c(TCOD_dark_red, "feel a bit more bummed than usual.");
}
void calculate_worldview(player_t *player)
{
        // basically: the more intelligence and knowledge you have, the greater the
        // chance that you are depressed.
        // and the stupider you are, the happier you are!

        player->wvfactor = (pintl*5 + pknow*5) / 2;

        if(player->wvfactor <= 45)
                player->worldview = 0;
        if(player->wvfactor > 45 && player->wvfactor <= 70)
                player->worldview = 1;
        if(player->wvfactor > 70)
                player->worldview = 2;
}

void init_player(player_t *player, int x, int y)
{
        int i;

        player->x = x;
        player->y = y;
        player->level = 1;
        player->ac = 0;
        player->next = player->prev = player->attacker = NULL;
        player->inventory = init_inventory(player->inventory);
        player->weapon = NULL;
        player->xp = 0;
        player->w.head = player->w.body = player->w.gloves = player->w.footwear = player->w.robe = player->w.amulet = NULL;
        player->flags = 0 | MF_ISPLAYER;
        for(i=0;i<10;i++) {
                player->skill[i] = 0.0;
                player->w.ring[i] = NULL;
        }

        if(!player->race)
                player->race = ri(1, RACES);
        if(!player->cla)
                player->cla = ri(1, CLASSES);
        pstr = pphys = pintl = pknow = pdex = pcha = 0;

        while(pstr < races[player->race].minstr)
                pstr =  dice(3, 6, races[player->race].str_mod + classes[player->cla].str_mod);
        while(pstr < classes[player->cla].minstr)
                pstr =  dice(3, 6, races[player->race].str_mod + classes[player->cla].str_mod);

        while((pphys < races[player->race].minphys) && (pphys < classes[player->cla].minphys))
                pphys = dice(3, 6, races[player->race].phys_mod + classes[player->cla].phys_mod);

        while((pintl < races[player->race].minintl) && (pintl < classes[player->cla].minintl))
                pintl = dice(3, 6, races[player->race].intl_mod + classes[player->cla].intl_mod);

        while((pknow < races[player->race].minknow) && (pknow < classes[player->cla].minknow))
                pknow = dice(3, 6, races[player->race].know_mod + classes[player->cla].know_mod);

        while((pdex < races[player->race].mindex) && (pdex < classes[player->cla].mindex))
                pdex  = dice(3, 6, races[player->race].dex_mod + classes[player->cla].dex_mod);

        while((pcha < races[player->race].mincha) && (pcha < classes[player->cla].mincha))
                pcha  = dice(3, 6, races[player->race].cha_mod + classes[player->cla].cha_mod);

        player->hp = get_init_hp(player);
        player->maxhp = player->hp;
        init_player_inventory(player);
        calculate_worldview(player);
        player->thac0 = (pdex/3) + (pstr/4);
}
