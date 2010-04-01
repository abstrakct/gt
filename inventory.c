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
        if(start->type == OT_GOLD && start->quantity)
                return start;
        if(start->type == OT_GOLD && start->quantity == 0)
                return start->next;

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

int movefromcelltoinventory(creature_t *creature, world_t *world)
{
        struct object *lastinpi, *previnpi, *which;

#define cellinv world->cell[creature->y][creature->x].inventory

        if(cellinv->type == OT_GOLD && cellinv->quantity) {
                creature->inventory->quantity += cellinv->quantity;
                cellinv->quantity = 0;
                return 0;
        }

        which = nextobjecthere->next;
        if(!which)  // nothing here
                return 1;

        lastinpi = creature->inventory->next;
        if(lastinpi) {
                while(lastinpi != NULL) {
                        previnpi = lastinpi;
                        lastinpi = lastinpi->next;
                }
        } else {
                previnpi = creature->inventory;
        }

        lastinpi = which;
        lastinpi->prev = previnpi;
        lastinpi->prev->next = lastinpi;

        cellinv->next = cellinv->next->next;
        cellinv->prev = cellinv;


        you_c(TCOD_green, "pick up %s %s.", identified(which->flags) ? a_an(which->fullname) : a_an(which->unidname), identified(which->flags) ? nouppercase(which->fullname) : nouppercase(which->unidname));
        uppercase(which->fullname);
        assign_objlet(which);
        return 0;
}

void movefrominventorytocell(player_t *creature, world_t *world, char c)
{
        struct object *previnpi;
        struct object *nextinpi;
        struct object *which;
        struct object *o, *t;

        which = get_obj_by_letter(c);
#define cellinv world->cell[creature->y][creature->x].inventory

        if(!cellinv) {  // has no inventory yet? allocate one then.
                cellinv = malloc(sizeof(struct object));
                cellinv->type = OT_GOLD;
                cellinv->quantity = 0;
                cellinv->next = cellinv->prev = NULL;
        }

        previnpi = which->prev;
        nextinpi = which->next;

        o = cellinv->next;
        t = cellinv;
        while(o) {
                t = o;
                o = o->next;
        }

        o = which;
        o->next = NULL;
        o->prev = t;
        t->next = o;

        unassign_objlet(which);
        you_c(TCOD_green, "drop %s.", nouppercase(which->fullname));
}


void movenode(struct object **source, struct object **dest)
{
        struct object *o = *source;

        *source = o->next;
        o->next = *dest;
        *dest = o;
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

        player->inventory->next->prev = player->inventory;
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
