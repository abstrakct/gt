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
        world->player->wvfactor--;
//        you("get %d xp!", get_xp(creature));
        creature->prev->next = creature->next;
        creature->next->prev = creature->prev;
        world->cell[creature->y][creature->x].monster = NULL;
        free(creature);
}

void attack(struct creature *attacker, struct creature *attackee, world_t *world)
{
        int damage = 1;
        int hit, i;
        int tohit, barehands;
        struct object *w;

        hit = dice(1, 20, 0); 
        tohit  = attacker->thac0;
        tohit -= attackee->ac;
        tohit += (attacker->worldview+1);

        barehands = 0;
        w = attacker->weapon;
        if(!w) {
                // bare hands!
                w = malloc(sizeof(struct object));
                w->ddice = 1;
                w->dsides = 4;
                w->modifier = 0;
                w->skill = SK_BAREHANDS;
                barehands = 1;
                attacker->weapon = w;
        }

        tohit += w->modifier;
        tohit += (int) attacker->skill[attacker->weapon->skill];

        if(tohit < 1)
                tohit = 1;

//        printf("hit = %d\nthac0 = %d, ac = %d, tohit = %d\n", hit, attacker->thac0, attackee->ac, tohit);
//        printf("wvfactor = %d\tworldview = %d\n", world->player->wvfactor, world->player->worldview);
        if(hit <= tohit) {
                if(barehands)
                       if(attacker == world->player)
                               world->player->xp += 2;    // award some experience fore bare hands fighting
                damage = dice(w->ddice, w->dsides, w->modifier);
                attackee->hp -= damage;
                if(attacker == world->player) {
                        you_c(TCOD_green, "hit the %s for %d %s of damage!", attackee->name, damage, damage == 1 ? "point" : "points");
                } else {
                        gtprintfc(TCOD_red, "The %s hits you for %d %s of damage!", attacker->name, damage, damage == 1 ? "point" : "points");
                        i = ri(1,100);
                        if(i <= world->player->wvfactor)
                                world->player->wvfactor++;

                }

                if(attacker->weapon->skill) {
                        attacker->skill[attacker->weapon->skill] += 0.1;
//                        gtprintf("%s - skill is now %f", attacker->name, attacker->skill[attacker->weapon->skill]);
                }


                if(attackee->hp <= 0) {
                        if(attacker == world->player) {
                                kill(attackee, world);
                        } else {
                                gtprintfc(TCOD_red, "Shit! You die!!");
                                world->player->hp += 2;
                        }
                }
        } else {
                if(attacker == world->player) {
                        you_c(TCOD_red, "miss the %s!", attackee->name);
                } else {
                        gtprintfc(TCOD_red, "The %s misses!", attacker->name, damage, damage == 1 ? "point" : "points");
                        i = ri(1,100);
                        if(i >= world->player->wvfactor)
                                world->player->wvfactor--;
                }

                if(attacker->weapon->skill) {
                        i = ri(1,100);
                        if(i<=(50+attacker->attr.intl)) {
                                attacker->skill[attacker->weapon->skill] += 0.1;
//                                gtprintf("%s - skill is now %f", attacker->name, attacker->skill[attacker->weapon->skill]);
                        }
                }

        }

        recalculate_worldview(world->player);
        if(barehands) {
                attacker->weapon = NULL;
                free(w);
        }

        return;
}
