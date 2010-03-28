#include <stdio.h>
#include <stdlib.h>
#include <libtcod.h>
#include "world.h" 
#include "creature.h"
#include "you.h"

void kill(struct creature *creature, world_t *world)
{
        you("kill the %s!", creature->name);
        creature->prev->next = creature->next;
        creature->next->prev = creature->prev;
        world->cell[creature->y][creature->x].monster = NULL;
        free(creature);
}

void attack(struct creature *attacker, struct creature *attackee, world_t *world)
{
        int damage = 1;

        attackee->hp -= damage;
        you("hit the %s for %d points of damage!", attackee->name, damage);

        if(attackee->hp <= 0)
                kill(attackee, world);

        return;
}
