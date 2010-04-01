/*
 *
 * functions for displaying stuff on the screen
 *
 */

#include <stdio.h>
#include <string.h>
#include <libtcod.h>
#include <stdarg.h>
#include <stdlib.h>

#include "gt.h"
#include "objects.h"
#include "world.h"
#include "creature.h"
#include "display.h"
#include "you.h"
#include "worldview.h"
#include "inventory.h"

extern race_t races[RACES];
extern class_t classes[CLASSES];

int uy, ux;

void draw_world_fov(int startx, int starty, player_t *player, int width, int height, world_t *world, TCOD_map_t fovmap)
{
        int i,j;
        int c;
        int dx,dy;

        for(j=starty,dy=0;j<(starty+height);j++,dy++) {
                for(i=startx,dx=0;i<(startx+width);i++,dx++) {
                        switch(world->cell[j][i].type) {
                                case VILLAGE_NOHOUSE:
                                case CITY_NOHOUSE:
                                case FOREST_NOTREE:
                                case PLAIN: 
                                        c = CHAR_PLAIN;
                                        break;
                                case FOREST:
                                        c = CHAR_FOREST;
                                        break;
                                        //              case D_WALL:
                                case VILLAGE:
                                case CITY:
                                        c = CHAR_CITY;
                                        break;
                                case DUNGEON:
                                        c = CHAR_DUNGEON;
                                        break;
/*                                case D_FLOOR:
                                        c = CHAR_FLOOR;
                                        break;*/
                        }



                        if(TCOD_map_is_in_fov(fovmap, i, j)) {
                                if(world->cell[j][i].inventory) {
                                        obj_t *t;
                                        //t = get_first_object(world->cell[j][i].inventory);
                                        t = world->cell[j][i].inventory->next;
                                        c = objchars[t->type];
                                        TCOD_console_set_foreground_color(map_console, TCOD_white);
                                } else if(world->cell[j][i].monster) {
                                        c = world->cell[j][i].monster->c;
                                        TCOD_console_set_foreground_color(map_console, TCOD_white);
                                } else {
                                        TCOD_console_set_foreground_color(map_console, world->cell[j][i].color);
                                }
                                TCOD_console_put_char(map_console, dx, dy, c, TCOD_BKGND_NONE);
                        } else {
                                TCOD_console_set_foreground_color(map_console, TCOD_black);
                                TCOD_console_put_char(map_console, dx, dy, c, TCOD_BKGND_NONE);
                        }

                        if(i == player->x) {
                                if(j == player->y) {
                                        c = CHAR_PLAYER;
                                        TCOD_console_set_foreground_color(map_console, COLOR_PLAYER);
                                        TCOD_console_put_char(map_console, dx, dy, c, TCOD_BKGND_NONE);
                                }
                        }
                }
        }
}

void draw_world(int startx, int starty, player_t *player, int width, int height, world_t *world)
{
        int i,j;
        int c;
        int dx,dy;

        for(j=starty,dy=0;j<(starty+height);j++,dy++) {
                for(i=startx,dx=0;i<(startx+width);i++,dx++) {
                        switch(world->cell[j][i].type) {
                                case VILLAGE_NOHOUSE:
                                case CITY_NOHOUSE:
                                case FOREST_NOTREE:
                                case PLAIN: 
                                        c = CHAR_PLAIN;
                                        break;
                                case FOREST:
                                        c = CHAR_FOREST;
                                        break;
                                        //              case D_WALL:
                                case VILLAGE:
                                case CITY:
                                        c = CHAR_CITY;
                                        break;
                                case DUNGEON:
                                        c = CHAR_DUNGEON;
                                        break;
/*                                case D_FLOOR:
                                        c = CHAR_FLOOR;
                                        break;*/
                        }

                        TCOD_console_set_foreground_color(map_console, world->cell[j][i].color);

                        if(world->cell[j][i].inventory) {
                                obj_t *t;
                                t = get_first_object(world->cell[j][i].inventory);
                                if(t) {
                                        c = objchars[t->type];
                                        if(t->type == OT_GOLD)
                                                TCOD_console_set_foreground_color(map_console, TCOD_orange);
                                        else
                                                TCOD_console_set_foreground_color(map_console, TCOD_white);
                                }
                        }

                        if(world->cell[j][i].monster) {
                                c = world->cell[j][i].monster->c;
                                TCOD_console_set_foreground_color(map_console, TCOD_white);
                        }

                        TCOD_console_put_char(map_console, dx, dy, c, TCOD_BKGND_NONE);

                        if(i == player->x) {
                                if(j == player->y) {
                                        c = CHAR_PLAYER;
                                        TCOD_console_set_foreground_color(map_console, COLOR_PLAYER);
                                        TCOD_console_put_char(map_console, dx, dy, c, TCOD_BKGND_NONE);
                                }
                        }
                }
        }
}

void draw_dungeon(int startx, int starty, player_t *player, int width, int height, world_t *world)
{
        int i,j;
        int c;
        int dx,dy;

        for(j=starty,dy=0;j<(starty+height);j++,dy++) {
                for(i=startx,dx=0;i<(startx+width);i++,dx++) {
                        switch(world->dungeon.cell[j][i].type) {
                                case D_WALL:
                                        c = CHAR_DWALL;
                                        break;
                                case D_FLOOR:
                                        c = CHAR_DFLOOR;
                                        break;
                        }

                        if(has_exit(i, j)) {
                                c = CHAR_DUNGEON;
                                TCOD_console_set_foreground_color(map_console, TCOD_yellow);
                        } else {
                                TCOD_console_set_foreground_color(map_console, world->dungeon.cell[j][i].color);
                        }

                        TCOD_console_put_char(map_console, dx, dy, c, TCOD_BKGND_NONE);

                        if(i == player->x) {
                                if(j == player->y) {
                                        c = CHAR_PLAYER;
                                        TCOD_console_set_foreground_color(map_console, COLOR_PLAYER);
                                        TCOD_console_put_char(map_console, dx, dy, c, TCOD_BKGND_NONE);
                                }
                        }
                }
        }
}

void draw_dungeon_fov(int startx, int starty, player_t *player, int width, int height, world_t *world, TCOD_map_t fovmap)
{
        int i,j;
        int c;
        int dx,dy;
        TCOD_color_t col;

        for(j=starty,dy=0;j<(starty+height);j++,dy++) {
                for(i=startx,dx=0;i<(startx+width);i++,dx++) {
                        switch(world->dungeon.cell[j][i].type) {
                                case D_WALL:
                                        c = CHAR_DWALL;
                                        break;
                                case D_FLOOR:
                                        c = CHAR_DFLOOR;
                                        break;
                        }

                        if(has_exit(i, j))
                                c = CHAR_DUNGEON;

                        if(TCOD_map_is_in_fov(fovmap, i, j) || (cell_explored(world->dungeon.cell[j][i]))) {
                                set_explored_dungeon_cell(i, j);
                                if(has_exit(i, j)) {
                                        col = TCOD_yellow;
                                } else {
                                        col = world->dungeon.cell[j][i].color;
                                }
                                TCOD_console_set_foreground_color(map_console, col);
                                TCOD_console_put_char(map_console, dx, dy, c, TCOD_BKGND_NONE);
                        } else {
                                TCOD_console_set_foreground_color(map_console, TCOD_black);
                                TCOD_console_put_char(map_console, dx, dy, c, TCOD_BKGND_NONE);
                        }


                        if(i == player->x) {
                                if(j == player->y) {
                                        c = CHAR_PLAYER;
                                        TCOD_console_set_foreground_color(map_console, COLOR_PLAYER);
                                        TCOD_console_put_char(map_console, dx, dy, c, TCOD_BKGND_NONE);
                                }
                        }
                }
        }
}

char *get_current_location_name(world_t *world)
{
        static char message[250];

        if(!world->current_city && !world->current_village && !world->current_forest) {
                if(outside)
                        sprintf(message, "The wild, wild world!");
                if(indungeon)
                        sprintf(message, "In a dark and scary dungeon!");
        }

        if(world->current_city)
                sprintf(message, "The city of %s", world->city[world->current_city].name);
        
        if(world->current_village)
                sprintf(message, "The village of %s", world->village[world->current_village].name);
        
        if(world->current_forest)
                sprintf(message, "The forest of %s", world->forest[world->current_forest].name);

        return message;
}

char* nouppercase(char *s)
{
        int i;

        for(i=0;i<strlen(s);i++) {
                if(s[i] >= 65 && s[i] <= 87) {
                        s[i] += 32;
                        return s;
                }
        }

        return s;
}

void update_info(int x, int y, world_t *world, player_t *player, bool seenothing)
{
        char m[250];
        obj_t *o;

        m[0] = '\0';

        /*        if(outside && !has_dungeon(player->x, player->y))
                  sprintf(m, "You are outside in the wild, wild, world! ");
                  if(indungeon)
                  mess("You are inside a dark and scary dungeon!");*/

        if(has_dungeon(player->x, player->y))
                mess_color(TCOD_yellow, "You see a dungeon entrance.");

        if(has_objects(player->x, player->y)) {
                o = world->cell[player->y][player->x].inventory;
                while(o != NULL) {
                        if(o->type == OT_GOLD && o->quantity)
                                yousee("%d %s of gold here.", o->quantity, o->quantity > 1 ? "pieces" : "piece");
                        if(o->type != OT_GOLD) {
                                if(identified(o->flags))
                                        yousee("%s %s here.", a_an(o->fullname), nouppercase(o->fullname));
                                else
                                        yousee("%s %s here.", a_an(o->unidname), nouppercase(o->unidname));
                        }
                        o = o->next;
                }
        } else if(world->cell[player->y][player->x].monster) {
                yousee("a %s here.", world->cell[player->y][player->x].monster->name);
        } else {
                if(!has_dungeon(player->x, player->y) && seenothing) {
                        mess(WV_SEENOTHING);
                }
        }

}

void youwinprint(TCOD_color_t color, char *fmt, ...)
{
        va_list argp;
        char s[1000];

        TCOD_console_set_foreground_color(NULL, color);

        va_start(argp, fmt);
        vsnprintf(s, 36, fmt, argp);
        va_end(argp);

        TCOD_console_print_left(NULL, ux, uy, TCOD_BKGND_NONE, s);
        uy++;
}

int display_inventory(player_t *player)
{
        obj_t *o;
        int ny;
        int wielded;

        ny=uy;
        wielded = 0;
        o = player->inventory->next;
        while(o != NULL) {
                if(o && o == player->weapon)
                        wielded = 1;
                if(identified(o->flags))
                        youwinprint(TCOD_white, "%c %s%s", o->c, wielded ? "* " : "- ", nouppercase(o->fullname));
                else
                        youwinprint(TCOD_white, "%c %s%s %s", o->c, wielded ? "* " : "- ", a_an(o->unidname), nouppercase(o->unidname));
                o = o->next;
                wielded = 0;
                ny++;
        }

        return ny;
}

void update_stats(int x, int y, world_t *world, player_t *player)
{
        //int i, j = 0;
        int tmpy;

/*        TCOD_console_print_left(NULL, x, y, TCOD_BKGND_NONE, "Gullible's Travails v%s", GT_VERSION);
        TCOD_console_print_left(NULL, x, y+1, TCOD_BKGND_NONE, "**************************");
        TCOD_console_print_left(NULL, x, y+2, TCOD_BKGND_NONE, "Cities: %d", world->cities);
        TCOD_console_print_left(NULL, x, y+3, TCOD_BKGND_NONE, "Villages: %d", world->villages);
        TCOD_console_print_left(NULL, x, y+4, TCOD_BKGND_NONE, "Forests: %d", world->forests);*/
        

        ux = x;
        uy = y;

        youwinprint(TCOD_yellow, "%s", player->name);
        youwinprint(TCOD_light_blue, "Level %d %s %s", player->level, races[player->race].name, classes[player->cla].name);

        uy++;
        youwinprint(TCOD_green, "Life points:      / %d", player->maxhp);
        if(player->hp >= perc(player->maxhp, 75))
                TCOD_console_set_foreground_color(NULL, TCOD_green);
        if(player->hp >= perc(player->maxhp, 30) && player->hp < perc(player->maxhp, 75))
                TCOD_console_set_foreground_color(NULL, TCOD_orange);
        if(player->hp >= 0 && player->hp < perc(player->maxhp, 30))
                TCOD_console_set_foreground_color(NULL, TCOD_red);
        TCOD_console_print_left(NULL, x+15, y+3, TCOD_BKGND_NONE, "%d", player->hp);

        youwinprint(TCOD_green,  "Armor class:   %d", player->ac);
        youwinprint(TCOD_green,  "Worldview:     %s", get_worldview_string(player));
        uy++;
        youwinprint(TCOD_yellow, "Experience:    %d", player->xp);
        youwinprint(TCOD_yellow, "Gold:          %d", player->inventory->quantity);
        uy++;

        youwinprint(TCOD_white, "Strength:      %d", pstr);
        youwinprint(TCOD_white, "Physique:      %d", pphys);
        youwinprint(TCOD_white, "Intelligence:  %d", pintl);
        youwinprint(TCOD_white, "Knowledge:     %d", pknow);
        youwinprint(TCOD_white, "Dexterity:     %d", pdex);
        youwinprint(TCOD_white, "Charisma:      %d", pcha);

        uy++;
        youwinprint(TCOD_green,  "Current location:");
        youwinprint(TCOD_yellow, "                                                    ");
        uy--;
        youwinprint(TCOD_yellow, "%s", get_current_location_name(world));

        uy=26;
        TCOD_console_rect(NULL, ux, uy, 35, 54, true, TCOD_BKGND_NONE); 
        youwinprint(TCOD_yellow, "Inventory:");
        tmpy = display_inventory(player);

/*        for(i=0;i<num_commands;i++) {
                if(strcmp(get_command_text(i), "")) {
                        youwinprint(TCOD_white, x, y+50+j, "%s", get_command_text(i));
                        j++;
                }
        }*/
}
