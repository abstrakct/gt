#include <stdio.h>
#include <stdlib.h>
#include <libtcod.h>

#include "gt.h"
#include "world.h" 
#include "creature.h"
#include "you.h"

void kill(struct creature *creature, world_t *world)
{
        you_c(TCOD_green, "kill the %s!", creature->name);
        creature->prev->next = creature->next;
        creature->next->prev = creature->prev;
        world->cell[creature->y][creature->x].monster = NULL;
        free(creature);
}

void attack(struct creature *attacker, struct creature *attackee, world_t *world)
{
        int damage = 1;
        int hit;

        hit = ri(1,100);

        if(hit >= 50) {
                damage += ri(0,3);
                attackee->hp -= damage;
                if(attacker == world->player)
                        you_c(TCOD_green, "hit the %s for %d %s of damage!", attackee->name, damage, damage == 1 ? "point" : "points");
                else
                        gtprintfc(TCOD_red, "The %s hits you for %d %s of damage!", attacker->name, damage, damage == 1 ? "point" : "points");


                if(attackee->hp <= 0) {
                        if(attacker == world->player) {
                                kill(attackee, world);
                        } else {
                                gtprintfc(TCOD_red, "Shit! You die!!");
                                world->player->hp += 2;
                        }
                }
        }

        return;
}
