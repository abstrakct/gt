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

void addbaseitemtoinventory(struct creature *creature, int i)
{
        obj_t *tmp;
        obj_t *first;

        tmp = creature->inventory;
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
        if(creature->flags & MF_ISPLAYER)
                assign_objlet(tmp);

        tmp->prev = first;
        tmp->next = NULL;
        tmp->flags |= OF_IDENTIFIED;
}

void addrandomnormalweapontoinventory(struct creature *c)
{
        obj_t *tmp;
        obj_t *first;
        int i;

        tmp = c->inventory;
        first = tmp;

        while(tmp != NULL) {
                first = tmp;
                tmp = tmp->next;
        }

        tmp = (obj_t *) malloc(sizeof(obj_t));

        if(!tmp)
                die("memory allocation error!");

        first->next = tmp;

        
        i = ri(FIRST_WEAPON, 6);  /* OBS HARDCODED !!!! */

        *tmp = objects[i];
        tmp->prev = first;
        tmp->next = NULL;
        tmp->flags |= OF_IDENTIFIED;
}

void addrandomnormalarmortoinventory(struct creature *c)
{
        obj_t *tmp;
        obj_t *first;
        int i;

        tmp = c->inventory;
        first = tmp;

        while(tmp != NULL) {
                first = tmp;
                tmp = tmp->next;
        }

        tmp = (obj_t *) malloc(sizeof(obj_t));

        if(!tmp)
                die("memory allocation error!");

        first->next = tmp;

        
        i = ri(FIRST_ARMOR, 21);    /* OBS HARDCODED !!! */

        *tmp = objects[i];
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
                addbaseitemtoinventory(player, LEATHER_SHOES);
        }

        if(is_fighter) {
                addbaseitemtoinventory(player, LONG_SWORD);
                addbaseitemtoinventory(player, CHAIN_MAIL);
                addbaseitemtoinventory(player, LEATHER_SHOES);
        }

        player->inventory->next->prev = player->inventory;
        player->inventory->type = OT_GOLD;
        player->inventory->quantity = ri(0, 50);



        //assletinv(player);  //heh.
}


void init_monster_inventory(struct creature *c)
{
//        obj_t *o;
        int i;        

        if(c->flags & MF_CANHAVEGOLD || c->flags & MF_CANUSEWEAPON || c->flags & MF_CANUSEARMOR) { // only add inventory for monsters who can have one!
                c->inventory = init_inventory(c->inventory);
                c->inventory->type = OT_GOLD;
                if(c->flags & MF_CANHAVEGOLD)
                        c->inventory->quantity = ri(1,100) * c->level;
                

                if(c->flags & MF_CANUSESIMPLESWORD) {
                        i = ri(1,100);
                        if(i<=33)
                                addbaseitemtoinventory(c, SHORT_SWORD);
                        if(i>33&&i<=66)
                                addbaseitemtoinventory(c, SMALL_AXE);
                        if(i>66)
                                addbaseitemtoinventory(c, DAGGER);
                }

                if(c->flags & MF_CANUSEWEAPON) {   /* give the monster an appropriate weapon... */
                        addrandomnormalweapontoinventory(c);
                        wieldwear(c->inventory->next, c, 0);
                }

                if(c->flags & MF_CANUSEARMOR) {
                        addrandomnormalarmortoinventory(c);
                        wieldwear(c->inventory->next->next, c, 0);
                }
        }

        /*printf("Added to inventory of %s:\n", c->name);
        o = c->inventory;
        while(o != NULL) {
                if(o->type == OT_GOLD) {
                        printf("\t%d pieces of gold\n", o->quantity);
                } else {
                        printf("\t%s\n", o->fullname);
                }
                o = o->next;
        }*/
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

void wieldwear(struct object *what, struct creature *creature, int isplayer)
{
        if(creature->inventory->next) {
                if(what) {
                        if(creature->weapon == what) {
                                if(isplayer)
                                        you("unwield the %s", creature->weapon->basename); 
                                creature->weapon->flags ^= OF_INUSE;
                                creature->weapon = NULL;  // unwield it!
                                return;
                        }

                        if(creature->w.body == what) {
                                if(isplayer)
                                        you("take off the %s", creature->w.body->basename);
                                creature->ac -= (creature->w.body->dsides + creature->w.body->modifier);
                                creature->w.body->flags ^= OF_INUSE;
                                creature->w.body = NULL;
                                return;
                        }

                        if(creature->w.head == what) {
                                if(isplayer)
                                        you("take off the %s", creature->w.head->basename);
                                creature->ac -= (creature->w.head->dsides + creature->w.head->modifier);
                                creature->w.head->flags ^= OF_INUSE;
                                creature->w.head = NULL;
                                return;
                        }
                        if(creature->w.gloves == what) {
                                if(isplayer)
                                        you("take off the %s", creature->w.gloves->basename);
                                creature->ac -= (creature->w.gloves->dsides + creature->w.gloves->modifier);
                                creature->w.gloves->flags ^= OF_INUSE;
                                creature->w.gloves = NULL;
                                return;
                        }
                        if(creature->w.footwear == what) {
                                if(isplayer)
                                        you("take off the %s", creature->w.footwear->basename);
                                creature->ac -= (creature->w.footwear->dsides + creature->w.footwear->modifier);
                                creature->w.footwear->flags ^= OF_INUSE;
                                creature->w.footwear = NULL;
                                return;
                        }

                        if(wearable(what)) {
                                if(is_bodywear(what->flags)) {
                                        if(creature->w.body) {
                                                if(isplayer)
                                                        gtprintfc(TCOD_red, "You must remove the %s before you can do that.", creature->w.body->basename);
                                                return;
                                        }
                                        creature->w.body = what;
                                        if(isplayer)
                                                you("put on the %s", creature->w.body->basename);
                                        creature->ac += creature->w.body->dsides + creature->w.body->modifier;
                                        creature->w.body->flags |= OF_INUSE;
                                } else if(is_footwear(what->flags)) {
                                        if(creature->w.footwear) {
                                                if(isplayer)
                                                        gtprintfc(TCOD_red, "You must take off the %s before you can do that.", creature->w.footwear->basename);
                                                return;
                                        }
                                        creature->w.footwear = what;
                                        if(isplayer)
                                                you("put on the %s", creature->w.footwear->basename);
                                        if((ri(0,100)) >= 59) {
                                                // get the reference? *g*
                                                if(isplayer)
                                                        mess("The shoes are too tight. You are unable to dance.");
                                        }
                                        creature->ac += creature->w.footwear->dsides + creature->w.footwear->modifier;
                                        creature->w.footwear->flags |= OF_INUSE;
                                } else if(is_gloves(what->flags)) {
                                        if(creature->w.gloves) {
                                                if(isplayer)
                                                        gtprintfc(TCOD_red, "You must take off the %s before you can do that.", creature->w.gloves->basename);
                                                return;
                                        }
                                        creature->w.gloves = what;
                                        if(isplayer)
                                                you("put on the %s", creature->w.gloves->basename);
                                        creature->ac += creature->w.gloves->dsides + creature->w.gloves->modifier;
                                        creature->w.gloves->flags |= OF_INUSE;
                                } else if(is_headwear(what->flags)) {
                                        if(creature->w.head) {
                                                if(isplayer)
                                                        gtprintfc(TCOD_red, "You must take off the %s before you can do that.", creature->w.head->basename);
                                                return;
                                        }
                                        creature->w.head = what;
                                        if(isplayer)
                                                you("put on the %s", creature->w.head->basename);
                                        creature->ac += creature->w.head->dsides + creature->w.head->modifier;
                                        creature->w.head->flags |= OF_INUSE;
                                }
                        }

                        if(wieldable(what)) {
                                creature->weapon = what;
                                creature->weapon->flags |= OF_INUSE;
                                if(isplayer)
                                        you("wield the %s", creature->weapon->basename);
                        }
                }
        }
}
