#ifndef WORLD_H
#define WORLD_H

/* Defines */

#define PLAIN		1
#define MOUNTAIN	2
#define FOREST		3
#define CITY		4
#define VILLAGE         5
#define CITY_NOHOUSE    6
#define VILLAGE_NOHOUSE 7
#define FOREST_NOTREE   8           /* this could lead to so many jokes... it's for marking a forest area without any actual tress. */
#define DUNGEON         9

#define MAXFORESTSIZE	70
#define LARGECITYSIZE	50
#define VILLAGESIZE	20
#define DUNGEONSIZE     10

#define XSIZE	800
#define YSIZE	800


/* DUNGEON STUFF */
#define D_FLOOR 100
#define D_WALL  101

#define mapheight YSIZE/2;
#define mapwidth  XSIZE/2;

#define CONTEXT_OUTSIDE 0
#define CONTEXT_DUNGEON 1


typedef struct {
	char type;
	TCOD_color_t color;
        int flags;
        struct object *inventory;
        struct creature *monster;
} cell_t;

#define IS_EXPLORED 0x0001
#define HAS_DUNGEON 0x0002
#define HAS_EXIT    0x0004

#define cell_explored(a) (a.flags & IS_EXPLORED)
#define has_dungeon(x, y) (world->cell[y][x].flags & HAS_DUNGEON)
#define has_exit(x, y) (world->dungeon.cell[y][x].flags & HAS_EXIT)
#define has_objects(x, y) (world->cell[y][x].inventory)

#define nextobjecthere world->cell[creature->y][creature->x].inventory
#define ccell world->cell[player->y][player->x]

typedef struct {
        int x1, y1, x2, y2;
        char name[40];
        int alignment;
        int houses;
} city_t;

typedef struct {
        int x1, y1, x2, y2;
        char name[25];
        int flags;
} forest_t;

typedef struct {
        int xsize, ysize;
        cell_t cell[YSIZE][XSIZE];
} dungeon_t;

typedef struct {
        cell_t cell[YSIZE][XSIZE];
        city_t city[50];
        city_t village[100];
        forest_t forest[100];
        dungeon_t dungeon;
        int villages, current_village;
        int cities, current_city;
        int forests, current_forest;
        int dungeons;
        int context;
        struct creature *player;
} world_t;

#define CURRENT_CITY (world->city[world->current_city])
#define CURRENT_VILLAGE (world->village[world->current_village])
#define CURRENT_FOREST (world->forest[world->current_forest])

#define outside (world->context == CONTEXT_OUTSIDE)
#define indungeon (world->context == CONTEXT_DUNGEON)
#define hit_wall (world->dungeon.cell[player->y][player->x].type == D_WALL)
#define set_explored_dungeon_cell(x, y) world->dungeon.cell[y][x].flags |= IS_EXPLORED

/* Prototypes */

void generate_area(int num, int type, int maxsize, int modifier, world_t *world);
void init_world(world_t *world);
void generate_world(world_t *world);

#endif
