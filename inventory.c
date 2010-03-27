/*
 * General inventory stuff!
 */

#include <stdlib.h>
#include <stdio.h>
#include <libtcod.h>

#include "gt.h"
#include "objects.h"
#include "objdefines.h"
#include "world.h"
#include "creature.h"
#include "you.h"
#include "display.h"
#include "inventory.h"

extern obj_t objects[];
obj_t *objlet[52];

#define OBJLET(a,b) objlet[letter_to_entry(a)] = b

int letter_to_entry(char c)
{
        int retval;

        retval = c;
        if(c >= 97 && c <= 122)
                retval -= 97;
        if(c >= 65 && c <= 90)
                retval -= 39;

        return retval;
}

char entry_to_letter(int i)
{
        int retval;

        retval = i;
        if(i >= 0 && i <= 25)
                retval += 97;
        if(i >= 26 && i < 52)
                retval += 39;

        return retval;
}

obj_t* get_obj_by_letter(char c)
{
        return objlet[letter_to_entry(c)];
}

char get_first_free_letter()
{
        int i;
        for(i=0;i<52;i++)
                if(!objlet[i])
                        return entry_to_letter(i);

        return 0; //No free object letter inventory ladida
}

void assign_objlet(obj_t *o)
{
        char c;

        c = get_first_free_letter();
        o->c = c;

        objlet[letter_to_entry(c)] = o;
}

void unassign_objlet(obj_t *o)
{
        int i;

        for(i=0;i<52;i++) {
                if(objlet[i] == o)
                        objlet[i] = NULL;
        }
}

void addbaseitemtoinventory(player_t *player, int i)
{
        obj_t *tmp;
        obj_t *first;

        tmp = player->inventory;
        first = tmp;

        while(tmp != NULL) {
                first = tmp;
                tmp = tmp->next;
        }

        tmp = (obj_t *) malloc(sizeof(obj_t));
        first->next = tmp;

        if(!tmp)
                die("memory allocation error!");

        *tmp = objects[i];
        assign_objlet(tmp);
        tmp->prev = first;
        tmp->next = NULL;
        tmp->flags |= OF_IDENTIFIED;
}

obj_t* get_last_object(obj_t *start)
{
        obj_t *tmp;

        tmp = start->next;
        while(tmp->next != NULL) {
                tmp = tmp->next;
        }

        return tmp;
}

struct object* get_first_object(obj_t *start)
{
        if(start->next)
                return start->next;
        else
                return start;
}

struct object* get_first_object_here(world_t *world, struct creature *creature)
{
        if(world->cell[creature->y][creature->x].inventory)
                return world->cell[creature->y][creature->x].inventory->next;

        return 0;
}

void movefromcelltoinventory(creature_t *creature, world_t *world)
{
        obj_t *tmp, *tmp2;

        if(world->cell[creature->y][creature->x].type == OT_GOLD) {
                creature->inventory->quantity += world->cell[creature->y][creature->x].inventory->quantity;
                world->cell[creature->y][creature->x].objects--;
                return;
        }

        tmp = creature->inventory->next;
        if(tmp) {
                while(tmp != NULL) {
                        tmp2 = tmp;
                        tmp = tmp->next;
                }
        } else {
                tmp2 = creature->inventory;
        }

        tmp = nextobjecthere;
        tmp2->next = tmp;
        tmp->prev = tmp2;
        you_c(TCOD_green, "pick up %s.", nouppercase(tmp->fullname));
        world->cell[creature->y][creature->x].objects--;
        uppercase(tmp->fullname);
        assign_objlet(tmp);
        tmp->next = NULL;
}

void movefrominventorytocell(creature_t *creature, world_t *world, char c)
{
        obj_t *tmp, *p;


        tmp = get_obj_by_letter(c);
        if(tmp == creature->inventory->next) {
                //this is the first item in the inventory
                tmp->prev = creature->inventory;
        }

        p = tmp->prev;
        if(p)
                p->next = tmp->next;
        if(tmp->next)
                tmp->next->prev = p;

        world->cell[creature->y][creature->x].objects++;
        world->cell[creature->y][creature->x].inventory = tmp; 

        unassign_objlet(tmp);
        you_c(TCOD_green, "drop %s.", nouppercase(tmp->fullname));
}

obj_t* init_inventory(obj_t *inventory)
{
        inventory = malloc(sizeof(obj_t));
        if(!inventory)
                die("couldn't allocate memory!");

        inventory->next = inventory->prev = NULL;
        return inventory;
}

void assletinv(player_t *player)
{
        obj_t *o;
        char let = 'a';

        o = player->inventory->next;
        while(o != NULL) {
                o->c = let;
                OBJLET(let, o);
                let++;
                o = o->next;
        }
}

void init_player_inventory(player_t *player)
{
        int i;

        for(i=0;i<52;i++)
                objlet[i] = NULL;

        if(is_wizard) {
                addbaseitemtoinventory(player, ri(FIRST_WAND, LAST_WAND));
                addbaseitemtoinventory(player, ri(FIRST_WAND, LAST_WAND));
        }

        if(is_fighter) {
                addbaseitemtoinventory(player, LONG_SWORD);
                addbaseitemtoinventory(player, CHAIN_MAIL);
        }

        player->inventory->next->prev = NULL;  // because the first in the list has no prev
        // but the first (player->inventory) is used to hold GOLD!

        player->inventory->type = OT_GOLD;
        player->inventory->quantity = ri(0, 50);



        //assletinv(player);  //heh.
}

void cleanup_inventory(player_t *player)
{
        int i;
        obj_t *o, *p;

        // find first object
        //

        i=0;
        while(i<52) {
                if(objlet[i]) {
                        player->inventory->next = objlet[i];
                        break;
                }
                i++;
        }

        if(i==52)  //no objects!
                player->inventory->next = NULL;

        o = player->inventory->next;
        if(!o)
                return;

        i++;
        while(i<52) {
                if(objlet[i]) {
                        p = o;
                        o->next = objlet[i];
                        o->prev = p;
                        o = o->next;
                }
                i++;
        }
        o->next = NULL;
}

void dump_inventory(obj_t *inventory)
{
        obj_t *o;

        o = inventory->next;
        while(o != NULL) {
                printf("  %s\n", o->fullname);
                o = o->next;
        }
}

void wield(obj_t *what, player_t *player)
{
        player->weapon = what;
}
