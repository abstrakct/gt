#include <stdio.h>
#include <stdlib.h>
#include <libtcod.h>

#include "gt.h"
#include "objects.h"
#include "world.h" 
#include "creature.h"
#include "you.h"

int get_xp(struct creature *creature)
{
        int ret;

        ret = creature->maxhp;
        ret += creature->ac;
        if(creature->attr.str >= 15)
                ret += creature->attr.str - 13;
        if(creature->attr.phys >= 15)
                ret += creature->attr.phys - 13;
        if(creature->attr.intl >= 15)
                ret += creature->attr.intl - 13;
        if(creature->attr.know >= 15)
                ret += creature->attr.know - 13;
        if(creature->attr.dex >= 15)
                ret += creature->attr.dex - 13;
        if(creature->attr.cha >= 15)
                ret += creature->attr.cha - 13;

        return ret;
}

void kill(struct creature *creature, world_t *world)
{
        you_c(TCOD_green, "kill the %s!", creature->name);
        world->player->xp += get_xp(creature);
        you("get %d xp!", get_xp(creature));
        creature->prev->next = creature->next;
        creature->next->prev = creature->prev;
        world->cell[creature->y][creature->x].monster = NULL;
        free(creature);
}

void attack(struct creature *attacker, struct creature *attackee, world_t *world)
{
        int damage = 1;
        int hit;
        int tohit, barehands;
        struct object *w;

        hit = dice(1, 20, 0); 
        tohit = attacker->thac0 - attackee->ac;
        barehands = 0;
        w = attacker->weapon;
        if(!w) {
                // bare hands!
                w = malloc(sizeof(struct object));
                w->ddice = 1;
                w->dsides = 4;
                w->modifier = 0;
                barehands = 1;
        }

        tohit += w->modifier;
        if(tohit < 1)
                tohit = 1;

//        printf("hit = %d\nthac0 = %d, ac = %d, tohit = %d\n", hit, attacker->thac0, attackee->ac, tohit);
        if(hit <= tohit) {
                if(barehands)
                       if(attacker == world->player)
                               world->player->xp += 2;    // award some experience fore bare hands fighting
                damage = dice(w->ddice, w->dsides, w->modifier);
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
        } else {
                if(attacker == world->player)
                        you_c(TCOD_red, "miss the %s!", attackee->name);
                else
                        gtprintfc(TCOD_red, "The %s misses!", attacker->name, damage, damage == 1 ? "point" : "points");
        }

        if(barehands)
                free(w);

        return;
}
