#ifndef _INVENTORY_H
#define _INVENTORY_H

/*
 * prototypes
 */


void addbaseitemtoinventory(struct creature *player, int i);
obj_t* get_last_object(obj_t *start);
struct object* get_first_object(obj_t *start);
struct object* get_first_object_here(world_t *world, struct creature *creature);
int movefromcelltoinventory(creature_t *creature, world_t *world);

//void movefrominventorytocell(creature_t *creature, world_t *world, char c);
void movefrominventorytocell(player_t *creature, world_t *world, char c);

obj_t* init_inventory(obj_t *inventory);
void init_player_inventory(player_t *player);
void dump_inventory(obj_t *inventory);
void cleanup_inventory(player_t *player);
void wieldwear(char what, player_t *creature);

int letter_to_entry(char c);
obj_t* get_obj_by_letter(char c);
void assign_objlet(obj_t *o);
void unassign_objlet(obj_t *o);
char get_first_free_letter();
void movenode(struct object **source, struct object **dest);

#endif
