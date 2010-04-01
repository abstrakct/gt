/*
 *
 * world->c
 *
 * world generation routines
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libtcod.h>

#include "gt.h"
#include "objects.h"
#include "objdefines.h"
#include "world.h"
#include "creature.h"
#include "display.h"
#include "namegen.h"

extern obj_t objects[];

void generate_area(int num, int type, int maxsize, int modifier, world_t *world)
{
        int i;

        TCOD_color_t forestcolors[5] = { COLOR_FOREST1, COLOR_FOREST2, COLOR_FOREST3, COLOR_FOREST4, COLOR_FOREST5 };
        TCOD_color_t citycolors[2] = { COLOR_CITY1, COLOR_CITY2 };
        TCOD_color_t dungeoncolors[3] = { TCOD_yellow, TCOD_dark_yellow, TCOD_light_yellow };

        for(i=1;i<num;i++) {
                int tx,ty,xsize,ysize;
                int fx,fy;
                int a, chance = 0;
                int csx, cex, csy, cey;
                float outerlinesx, outerlinesy;
                int edgex, edgey;
                int color;

                tx = ri(0, XSIZE-1);   // tx = areax 0 (...)
                ty = ri(0, YSIZE-1);
                xsize = ri(modifier, maxsize-1+modifier);
                ysize = ri(modifier, maxsize-1+modifier);

                // let's not go over the edge
                if(tx+xsize >= XSIZE)
                        xsize = XSIZE - tx;
                if(ty+ysize >= YSIZE)
                        ysize = YSIZE - ty;

                csx = tx + (xsize/2) - (xsize/4);
                cex = tx + (xsize/2) + (xsize/4);
                csy = ty + (ysize/2) - (ysize/4);
                cey = ty + (ysize/2) + (ysize/4);

                outerlinesx = ((float) xsize / 100) * 10;
                outerlinesy = ((float) ysize / 100) * 10;
                edgex = (int) outerlinesx;
                edgey = (int) outerlinesy;
                if(edgex == 0)
                        edgex = 1;
                if(edgey == 0)
                        edgey = 1;

#ifdef DEBUG_ON
                printf("\n\t\tcs = %d x %d  ce = %d x %d\n", csx, csy, cex, cey);
                printf("\t\tedgex = %d edgey = %d", edgex, edgey);

                switch(type) {
                        case FOREST:
                                printf("\n\t\tforest %d at %d x %d (%d %d)", i+1,tx,ty,xsize,ysize);
                                break;
                        case CITY:
                                printf("\n\t\tcity %d at %d x %d (%d %d)", i, tx, ty, xsize, ysize);
                                break;
                        default:
                                printf("\n\t\tunknown area type %d at %d x %d (%d %d)", type, tx, ty, xsize, ysize);
                                break;
                }
#endif
                if(type == CITY && i) {
                        world->city[i].x1 = tx;
                        world->city[i].y1 = ty;
                        world->city[i].x2 = tx+xsize-1;
                        world->city[i].y2 = ty+ysize-1;
                        world->city[i].houses = 0;
                        //sprintf(city[i].name, "Glorious City No. %d", i);
                        generate_name(NAME_CITY, world->city[i].name);
                        for(fy=ty;fy<(ty+ysize);fy++)
                                for(fx=tx;fx<(tx+xsize);fx++)
                                        if(world->cell[fy][fx].type == PLAIN)
                                                world->cell[fy][fx].type = CITY_NOHOUSE;


                        //        fprintf(f, "City %d\n\tx1,y1 = %d,%d\n\tx2,y2 = %d,%d\n\tname = %s\n", i, city[i].x1, city[i].y1, city[i].x2, city[i].y2, city[i].name);
                }

                if(type == VILLAGE && i) {
                        world->village[i].x1 = tx;
                        world->village[i].y1 = ty;
                        world->village[i].x2 = tx+xsize-1;
                        world->village[i].y2 = ty+ysize-1;
                        world->village[i].houses = 0;
                        generate_name(NAME_CITY, world->village[i].name);
                        for(fy=ty;fy<(ty+ysize);fy++)
                                for(fx=tx;fx<(tx+xsize);fx++)
                                        if(world->cell[fy][fx].type == PLAIN)
                                                world->cell[fy][fx].type = VILLAGE_NOHOUSE;

                }

                if(type == FOREST && i) {
                        world->forest[i].x1 = tx;
                        world->forest[i].y1 = ty;
                        world->forest[i].x2 = tx+xsize-1;
                        world->forest[i].y2 = ty+ysize-1;
                        world->forest[i].flags = 0;
                        generate_name(NAME_FOREST, world->forest[i].name);
                        for(fy=ty;fy<(ty+ysize);fy++)
                                for(fx=tx;fx<(tx+xsize);fx++)
                                        world->cell[fy][fx].type = FOREST_NOTREE;

                }

                if(type == DUNGEON)
                        for(fy=ty;fy<(ty+ysize);fy++)
                                for(fx=tx;fx<(tx+xsize);fx++)
                                        world->cell[fy][fx].flags |= HAS_DUNGEON;

                for(fy=ty;fy<ty+ysize;fy++) {
                        for(fx=tx;fx<tx+xsize;fx++) {

                                a = ri(1,100);

                                chance = 60;
                                // ensure less chance of trees at the edge of the forest and greater chance around the center
                                //if(((fx == (tx-(xsize/2))) || (fx == (tx-(xsize/2)+1))) && ((fy == (ty-(ysize/2))) || (fy == (ty-(ysize/2)+1))))
                                if(fx <= tx+edgex)
                                        chance = 90;
                                if(fy <= ty+edgey)
                                        chance = 90;
                                if(fy >= ty+ysize-edgey)
                                        chance = 90;
                                if(fx >= tx+xsize-edgex)
                                        chance = 90;
                                if(fx >= csx && fx <= cex && fy >= csy && fy <= cey) {
                                        if(type == CITY || type == VILLAGE) {
                                                chance = 0;
                                        } else if(type == FOREST) {
                                                chance = 20;
                                        }
                                }

                                if(a >= chance) {
                                        world->cell[fy][fx].type = type;
                                        switch(type) {
                                                case FOREST:
                                                        color = ri(0,4);
                                                        world->cell[fy][fx].color = forestcolors[color];
                                                        break;
                                                case VILLAGE:
                                                        color = ri(0,1); 
                                                        world->cell[fy][fx].color = citycolors[color];
                                                        world->village[i].houses++;
                                                        break;
                                                case CITY:
                                                        color = ri(0,1);
                                                        world->cell[fy][fx].color = citycolors[color];
                                                        world->city[i].houses++;
                                                        break;
                                                case DUNGEON:
                                                        color = ri(0,2);
                                                        world->cell[fy][fx].color = dungeoncolors[color];
                                                        world->dungeons++;
                                                        world->cell[fy][fx].flags |= HAS_DUNGEON;
                                                        break;
                                        }
                                }
                        }
                }
        }
}

void addgoldtoworldcell(world_t *world, int x, int y, int max)
{
        obj_t *tmp;

        tmp = world->cell[y][x].inventory;
        if(!tmp) {
                tmp = malloc(sizeof(struct object));
                if(!tmp)
                        die("memory allocation failed!");
        }
        tmp->prev = NULL;
        tmp->next = NULL;
        tmp->type = OT_GOLD;

        if(!max)
                tmp->quantity = 0;
        else
                tmp->quantity = ri(1, max);

        world->cell[y][x].inventory = tmp;
}

void addbaseitemtoworldcell(world_t *world, int x, int y, int i)
{
        obj_t *tmp;
        obj_t *first;

        tmp = world->cell[y][x].inventory;
        first = tmp;
        while(tmp != NULL) {
                first = tmp;
                tmp = tmp->next;
        }
        
        tmp = malloc(sizeof(obj_t));

        if(!tmp)
                die("memory allocation error!");
 
        if(first)
                first->next = tmp;

        *tmp = objects[i];
        tmp->prev = first;
        tmp->next = NULL;
        world->cell[y][x].inventory->next = tmp;
}


void generate_world(world_t *world)
{
        int i,j,color,items,x,y;
        TCOD_color_t plaincolors[3] = { COLOR_PLAIN1, COLOR_PLAIN2, COLOR_PLAIN3 };

        world->cities = world->villages = world->forests = world->dungeons = 0;

        for(i=0;i<XSIZE;i++) {
                for(j=0;j<YSIZE;j++) { 
                        world->cell[j][i].type = PLAIN;
                        world->cell[j][i].flags = 0;
                        color = ri(0,2);
                        world->cell[j][i].color = plaincolors[color];
                        world->cell[j][i].monster = NULL;
                        world->cell[j][i].inventory = NULL;
                }
        }

        // now, lets generate some stuff

        world->forests = ri(10,90);
        printf("\n\tgenerating %d forests...", world->forests);
        generate_area(world->forests, FOREST, MAXFORESTSIZE, 4, world);

        world->cities = ri(15,35);
        printf("\n\tgenerating %d large cities...", world->cities);
        generate_area(world->cities, CITY, LARGECITYSIZE, 2, world);

        world->villages = world->cities + ri(30,100); 
        printf("\n\tgenerating %d villages...\n", world->villages);
        generate_area(world->villages, VILLAGE, VILLAGESIZE, 1, world);

        world->dungeons = ri(50,100);
        printf("\n\treadying %d dungeons...\n", world->dungeons);
        generate_area(world->dungeons, DUNGEON, DUNGEONSIZE, 0, world);

        items = ri(50, 130);
        printf("\n\tdistributing %d items around the world...\n", items);
        for(i=0;i<items;i++) {
                j = ri(0, NUM_OBJECTS-1);
                while(objects[j].flags & OF_DONOTUSE)
                        j = ri(0, NUM_OBJECTS-1);

                if(objects[j].unique) {
                        if(objects[j].quantity<=1) {
                                x = ri(0,XSIZE-1);
                                y = ri(0,YSIZE-1);
                                addgoldtoworldcell(world, x, y, 0);       // add a dummy base gold item thingy
                                addbaseitemtoworldcell(world, x, y, j);
                                objects[j].quantity++;
                                printf("added %s to the world\n", objects[j].fullname);
                        }
                } else {
                        x = ri(0,XSIZE-1);
                        y = ri(0,YSIZE-1);
                        addgoldtoworldcell(world, x, y, 0);       // add a dummy base gold item thingy
                        addbaseitemtoworldcell(world, x, y, j);
                        //printf("added %s to the world\n", objects[j].fullname);
                }

                if(ri(1,100) >= 50) {
                        addgoldtoworldcell(world, ri(0, XSIZE-1), ri(0, YSIZE-1), 200);
                        //printf("adding some gold!\n");
                }
        }
}

