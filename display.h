#ifndef DISPLAY_H
#define DISPLAY_H

#define CHAR_PLAIN  0xFA
#define COLOR_PLAIN1 TCOD_dark_chartreuse
#define COLOR_PLAIN2 TCOD_desaturated_chartreuse
#define COLOR_PLAIN3 TCOD_darker_chartreuse

#define CHAR_FOREST  0x06
#define COLOR_FOREST TCOD_green

#define COLOR_FOREST1 TCOD_green
#define COLOR_FOREST2 TCOD_desaturated_green
#define COLOR_FOREST3 TCOD_light_green
#define COLOR_FOREST4 TCOD_dark_green
#define COLOR_FOREST5 TCOD_darker_green

#define CHAR_CITY   0x7F
#define COLOR_CITY  TCOD_dark_orange
#define COLOR_CITY1 TCOD_dark_orange
#define COLOR_CITY2 TCOD_orange

#define CHAR_DUNGEON 0x09
#define CHAR_DFLOOR  0x2E
#define CHAR_DWALL   0x23

#define CHAR_PLAYER 0x40
#define COLOR_PLAYER TCOD_yellow

extern TCOD_console_t map_console;

void draw_world_fov(int startx, int starty, player_t *player, int width, int height, world_t *world, TCOD_map_t fovmap);
void draw_world(int startx, int starty, player_t *player, int width, int height, world_t *world);
void draw_dungeon(int startx, int starty, player_t *player, int width, int height, world_t *world);
void draw_dungeon_fov(int startx, int starty, player_t *player, int width, int height, world_t *world, TCOD_map_t fovmap);
void update_info(int x, int y, world_t *world, player_t *player, bool seenothing);
void update_stats(int x, int y, world_t *world, player_t *player);
char* nouppercase(char *s);

#endif
