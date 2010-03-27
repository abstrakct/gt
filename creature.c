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

creature_t *mon;

signed int hp_phys_adj[] = { 0, -3, -2, -2, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 3, 4, 5 };

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
                13, 14, 7, 7, 9, 7},
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

void hostileai(creature_t *creature, creature_t *player)
{
        creature->goalx = player->x;
        creature->goaly = player->y;

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
                tmp->movement += tmp->speed;
                while(tmp->movement >= 1.0) {
                        world->cell[tmp->y][tmp->x].monster = NULL;
                        tmp->ai(tmp, player);
                        world->cell[tmp->y][tmp->x].monster = tmp;
                        tmp->movement--;
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

        // let's populate the world!

        tmp = mon;
        num = ri(50,150);
        printf("creating %d monsters..\n", num);

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
                //printf("\tcreated a %s\n", tmp->next->name);
                tmp = tmp->next;
                tmp->x = ri(0,XSIZE-1);
                tmp->y = ri(0,YSIZE-1);
                tmp->next = NULL;
                world->cell[tmp->y][tmp->x].monster = tmp;
        }

        return;
}



int get_init_hp(player_t *player)
{
        return dice(1, 10, 10+hp_phys_adj[pphys]);
}

void init_player(player_t *player, int x, int y)
{
        //memset(player, 0, sizeof(player_t));
        player->x = x;
        player->y = y;
        player->level = 1;

        player->inventory = init_inventory(player->inventory);
        player->weapon = NULL;
        player->xp = 0;

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
        player->worldview = ri(0,2);
}
