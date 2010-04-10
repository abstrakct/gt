#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>
#include <string.h>
#include <libtcod.h>

#include "world.h"
#include "creature.h"
#include "cmd.h"
#include "worldview.h"
#include "gt.h"
#include "objects.h"
#include "display.h"
#include "dungeon.h"
#include "heightmap.h"
#include "cards.h"
#include "you.h"
#include "inventory.h"
#include "attack.h"
#include "materials.h"

/* define this if development mode!! */
#define DEVEL_MODE

int px,py;

FILE *debuglog;
TCOD_console_t map_console;
TCOD_console_t mess_console;

extern race_t races[RACES];
extern class_t classes[CLASSES];
extern char *skillstr[];

int mapcxsize, mapcysize;

bool seenothing;

int currmess;
struct message {
        TCOD_color_t color;
        char text[250];
};

TCOD_random_t rndgen;

struct message m[MAX_MESS];
player_t savedplayer;

int get_city_at(int x, int y, world_t *world)
{
        int i, ret;

        for(i=1;i<world->cities;i++) {
                if(x >= world->city[i].x1)
                        if(x <= world->city[i].x2)
                                if(y >= world->city[i].y1)
                                        if(y <= world->city[i].y2) {
                                                ret = i;
                                                i=world->cities;
                                        }
        }

        return ret;
}

int get_village_at(int x, int y, world_t *world)
{
        int i, ret;

        for(i=1;i<world->villages;i++) {
                if(x >= world->village[i].x1)
                        if(x <= world->village[i].x2)
                                if(y >= world->village[i].y1)
                                        if(y <= world->village[i].y2) {
                                                ret = i;
                                                i=world->villages;
                                        }
        }

        return ret;
}

int get_forest_at(int x, int y, world_t *world)
{
        int i, ret;

        for(i=1;i<world->forests;i++) {
                if(x >= world->forest[i].x1)
                        if(x <= world->forest[i].x2)
                                if(y >= world->forest[i].y1)
                                        if(y <= world->forest[i].y2) {
                                                ret = i;
                                                i=world->forests;
                                        }
        }

        return ret;
}

void check_location(player_t *player, world_t *world)
{
        world->current_city = world->current_village = world->current_forest = 0;

        if(world->cell[player->y][player->x].type == CITY || world->cell[player->y][player->x].type == CITY_NOHOUSE)
                world->current_city = get_city_at(player->x, player->y, world);

        if(world->cell[player->y][player->x].type == VILLAGE || world->cell[player->y][player->x].type == VILLAGE_NOHOUSE)
                world->current_village = get_village_at(player->x, player->y, world);

        if(world->cell[player->y][player->x].type == FOREST || world->cell[player->y][player->x].type == FOREST_NOTREE)
                world->current_forest = get_forest_at(player->x, player->y, world);
}

void set_explored_city(TCOD_map_t fovmap, world_t *world)
{
        int x, y;

        for(x=CURRENT_CITY.x1;x<=CURRENT_CITY.x2;x++) {
                for(y=CURRENT_CITY.y1;y<=CURRENT_CITY.y2;y++) {
                        TCOD_map_set_properties(fovmap, x, y, true, true);
                }
        }
}

void set_explored_village(TCOD_map_t fovmap, world_t *world)
{
        int x, y;

        for(x=CURRENT_VILLAGE.x1;x<=CURRENT_VILLAGE.x2;x++) {
                for(y=CURRENT_VILLAGE.y1;y<=CURRENT_VILLAGE.y2;y++) {
                        TCOD_map_set_properties(fovmap, x, y, true, true);
                }
        }
}

void set_explored_forest(TCOD_map_t fovmap, world_t *world)
{
        int x, y;

        for(x=CURRENT_FOREST.x1;x<=CURRENT_FOREST.x2;x++) {
                for(y=CURRENT_FOREST.y1;y<=CURRENT_FOREST.y2;y++) {
                        TCOD_map_set_properties(fovmap, x, y, true, true);
                }
        }
}

void save_world(world_t *world, player_t *player, char *filename)
{
        FILE *worldfile;
        //int i, j;

        worldfile = fopen(filename, "wb");
        
        fwrite(world, sizeof(world_t), 1, worldfile);
        fwrite(player, sizeof(player_t), 1, worldfile);

        fclose(worldfile);
}

int load_world(world_t *world, player_t *player, char *filename)
{
        FILE *worldfile;

        worldfile = fopen(filename, "r");
        if(worldfile == NULL) {
                return 1;
        }

        fread(world, sizeof(world_t), 1, worldfile);
        fread(player, sizeof(player_t), 1, worldfile);
        fclose(worldfile);
        return 0;
}

void show_welcome_screen(int wmaxx, int wmaxy)
{
        TCOD_console_print_center(NULL, wmaxx/2, (wmaxy/2), TCOD_BKGND_NONE, "Gullible's Travails v%s", GT_VERSION);
        TCOD_console_print_center(NULL, wmaxx/2, (wmaxy/2)+1, TCOD_BKGND_NONE, "Powered by libtcod %s", TCOD_STRVERSION);
}

void die(char *message)
{
        fprintf(stderr, "%s\n", message);

        exit(1);
}

void dump_heightmap(TCOD_heightmap_t *hm)
{
        int x, y;
        float val;
        FILE *f;
        
        f=fopen("hmapdump", "w");

        for(x = 0; x < XSIZE; x++) {
                for(y = 0; y < YSIZE; y++) {
                        val = TCOD_heightmap_get_value(hm, x, y);
                        //fprintf(f, "%1.1f ", val);
                        if((int)val<2)
                                fprintf(f, "v");
                        if(((int)val>=2) && ((int)val)<=9)
                                fprintf(f, ".");
                        if(((int)val)>9)
                                fprintf(f, "^");
//                        fprintf(f, "%d ", (int)(val));
                }
                fprintf(f, "\n");
        }
        fclose(f);
}

void get_dungeon_starting_location(player_t *player, world_t *world)
{
        int x, y;

        x = ri(0, world->dungeon.xsize-1);
        y = ri(0, world->dungeon.ysize-1);

        while(world->dungeon.cell[y][x].type == D_WALL) {
                x = ri(0, world->dungeon.xsize-1);
                y = ri(0, world->dungeon.ysize-1);
        }

        player->x = x;
        player->y = y;
}

void init_fov_world(world_t *world, TCOD_map_t fovmap)
{
        int x, y;

        for(x = 0; x < XSIZE; x++) {
                for(y = 0; y < YSIZE; y++) {
                        switch(world->cell[y][x].type) {
                                case VILLAGE_NOHOUSE:
                                case CITY_NOHOUSE:
                                case FOREST_NOTREE:
                                case PLAIN: 
                                        TCOD_map_set_properties(fovmap, x, y, true, true); 
                                        break;
                                case VILLAGE:
                                case CITY:
                                case FOREST:
                                        TCOD_map_set_properties(fovmap, x, y, false, true);
                                        break;
                        }
                }
        }
}

void init_fov_dungeon(world_t *world, TCOD_map_t fovmap)
{
        int x, y;

        for(x = 0; x < world->dungeon.xsize; x++) {
                for(y = 0; y < world->dungeon.ysize; y++) {
                        switch(world->dungeon.cell[y][x].type) {
                                case D_FLOOR: 
                                        TCOD_map_set_properties(fovmap, x, y, true, true); 
                                        break;
                                case D_WALL:
                                        TCOD_map_set_properties(fovmap, x, y, false, true);
                                        break;
                        }
                }
        }
}

int dice(int num, int sides, signed int modifier)
{
        signed int retval, i;

        retval = modifier;
        for(i=0;i<num;i++)
                retval += ri(1, sides);

        return retval;
}

void get_raceclass_selection(player_t *player)
{
        int i;
        char c;

        printf("Welcome to Gullible's Travails v%s\n\nChoose your race:\n", GT_VERSION);
        for(i=1;i<=RACES;i++) {
                printf("%d - %s\n", i, races[i].name);
        }
        printf("0 - random\n");

        c = getchar();
        player->race = atoi(&c);

        getchar();
        printf("\nChoose your class:\n");

        for(i=1;i<=CLASSES;i++) {
                printf("%d - %s\n", i, classes[i].name);
        }
        printf("0 - random\n");

        c = getchar();
        player->cla = atoi(&c);
        getchar();

        printf("\nWhat's your name? ");
        scanf("%s", player->name);
}

void domess()
{
        int i;

        currmess++;

        TCOD_console_clear(mess_console);
        TCOD_console_set_foreground_color(mess_console, TCOD_white);
        TCOD_console_print_frame(mess_console, 0, 0, mapcxsize+2, MAX_MESS+2, true, TCOD_BKGND_NONE, "Messages");
        for(i=MAX_MESS-1;i>=0;i--) {
                TCOD_console_set_foreground_color(mess_console, m[i].color);
                TCOD_console_print_left(mess_console, 1, i+1, TCOD_BKGND_NONE, m[i].text);
        }
        TCOD_console_blit(mess_console, 0, 0, mapcxsize+2, MAX_MESS+2, NULL, 0, mapcysize+2, 1.0, 1.0);
        TCOD_console_flush(NULL);
}

void mess(char *message)
{
        int i;

        if(!strcmp(message, m[currmess-1].text))
                return;

        if(currmess >= MAX_MESS) {
                currmess = MAX_MESS-1;
                for(i=0;i<=currmess;i++) {
                        m[i].color = m[i+1].color;
                        strcpy(m[i].text, m[i+1].text);
                }
        }

//        currmess++;
        m[currmess].color = TCOD_white;
        strcpy(m[currmess].text, message);
        domess();
}

void mess_color(TCOD_color_t color, char *message)
{
        int i;

        if(!strcmp(message, m[currmess-1].text))
                return;

        if(currmess >= MAX_MESS) {
                currmess = MAX_MESS-1;
                for(i=0;i<=currmess;i++) {
                        m[i].color = m[i+1].color;
                        strcpy(m[i].text, m[i+1].text);
                }
        }

//        currmess++;
        m[currmess].color = color;
        strcpy(m[currmess].text, message);
        domess();
}

void showskills(TCOD_console_t *cons, struct creature *player)
{
        int i, j, n;

        TCOD_console_print_left(*cons, 0, 0, TCOD_BKGND_NONE, "Skills:");

        for(i=1;i<=NUMSKILLS;i++) {
                TCOD_console_print_left(*cons, 0, i+1, TCOD_BKGND_NONE, "%s", skillstr[i]);
        }

        for(i=1;i<=NUMSKILLS;i++) {
                TCOD_console_print_left(*cons, 20, i+1, TCOD_BKGND_NONE, "[");
                n = (int) player->skill[i] / 10;
                for(j=0;j<n;j++)
                        TCOD_console_print_left(*cons, 21+j, i+1, TCOD_BKGND_NONE, "%c", 0xFE);
                for(j=n;j<10;j++)
                        TCOD_console_print_left(*cons, 21+j, i+1, TCOD_BKGND_NONE, ".");

                TCOD_console_print_left(*cons, 31, i+1, TCOD_BKGND_NONE, "]");
        }

}

void savepxy(player_t *player)
{
        savedplayer.x = player->x;
        savedplayer.y = player->y;
}

void loadpxy(player_t *player)
{
        player->x = savedplayer.x;
        player->y = savedplayer.y;
}

int perc(int n, int p)
{
        float ret;

        ret = (float) n;
        ret /= 100;
        ret *= p;

        return (int) ret;
}

char askplayer(char *question)
{
        TCOD_key_t k;
        mess_color(TCOD_green, question);
        k = TCOD_console_wait_for_keypress(false);
        if(k.vk != TCODK_CHAR)
                return 0;

        return k.c;
}

int main(int argc, char *argv[])
{
        int wmaxx, wmaxy/*, tx, ty*/;
        int mapchanged = 1;
        int mapsx, fov, d, c;
        player_t *player;
        world_t *world;
        int oldx, oldy;
        char wintitle[50];
        struct object *o;
        char what;
        TCOD_console_t tmpconsole;
        
        //TCOD_heightmap_t *hm;

        rndgen = TCOD_random_new(TCOD_RNG_MT);

        player = malloc(sizeof(player_t));
        if(!player)
                die("Initialization of player failed.");

#ifdef DEVEL_MODE
        player->x = XSIZE/2;
        player->y = YSIZE/2;
        player->race = player->cla = 1;
        strcpy(player->name, "Apsalar");
#else
        get_raceclass_selection(player);
#endif

        init_objects();
        init_materials();
        init_player(player, XSIZE/2, YSIZE/2);

        world = malloc(sizeof(world_t));
        if(!world)
                die("Couldn't allocate memory for the world!");

        wmaxx = 125;
        wmaxy = 80;

        mapcxsize = (int) ((wmaxx/3)*2) + 2;
        mapcysize = (int) ((wmaxy/3)*2) + 2;
        mapsx = (wmaxx-mapcxsize)/2;

        px = player->x - (mapcxsize/2);
        py = player->y - (mapcysize/2);

        currmess = MAX_MESS-1;
        for(d=0;d<MAX_MESS;d++) {
                m[d].color = TCOD_white;
                strcpy(m[d].text, " ");
        }

        sprintf(wintitle, "Gullible's Travails v%s", GT_VERSION);
        TCOD_console_set_custom_font("gt.png", TCOD_FONT_LAYOUT_ASCII_INROW, 16, 16);
        TCOD_console_init_root(wmaxx, wmaxy, wintitle, false);
        map_console = TCOD_console_new(mapcxsize, mapcysize);
        mess_console = TCOD_console_new(mapcxsize+2, MAX_MESS+2);
        show_welcome_screen(wmaxx, wmaxy);
        TCOD_console_flush();
        TCOD_console_set_foreground_color(NULL, TCOD_white);
        

        if(argc > 1) {
                if(!strcmp(argv[1], "-l")) {
                        TCOD_console_print_center(NULL, wmaxx/2, (wmaxy/2)+3, TCOD_BKGND_NONE, "Opening world file %s...", argv[2]);
                        if(load_world(world, player, argv[2]))
                                die("Couldn't open world file!");
                } else {
                        fprintf(stderr, "%s - ", argv[1]);
                        die("unknown argument!");
                }
        } else if(argc == 1) {
                TCOD_console_print_center(NULL, wmaxx/2, (wmaxy/2)+3, TCOD_BKGND_NONE, "Creating the world...");
                TCOD_console_flush();
                generate_world(world);
        }

        world->player = player;
        TCOD_console_print_center(NULL, wmaxx/2, (wmaxy/2)+4, TCOD_BKGND_NONE, "Creating a dungeon...");
        //init_dungeon(world, mapcxsize, mapcysize);

        TCOD_console_print_center(NULL, wmaxx/2, (wmaxy/2)+5, TCOD_BKGND_NONE, "Generating FOV map...");
        TCOD_console_flush();
        player->fov = TCOD_map_new(XSIZE, YSIZE);
        init_fov_world(world, player->fov);

        fov = 0;

        /*TCOD_console_print_center(NULL, wmaxx/2, (wmaxy/2)+6, TCOD_BKGND_NONE, "Generating heightmap...");
        TCOD_console_flush();
        hm = TCOD_heightmap_new(XSIZE, YSIZE);
        generate_heightmap(hm);
        TCOD_heightmap_normalize(hm, 0.0, 10.0);
        dump_heightmap(hm);*/

        generate_deck();
        init_monsters(world, player);

        TCOD_console_print_center(NULL, wmaxx/2, (wmaxy/2)+7, TCOD_BKGND_NONE, "All done! Press any key!");
        TCOD_console_flush();

        TCOD_console_print_frame(NULL, 0, 0, mapcxsize+2, mapcysize+2, true, TCOD_BKGND_NONE, "Map");

        world->context = CONTEXT_OUTSIDE;

        init_commands();
        while(1) {
                seenothing = 1;
                c = get_command();
                oldx = player->x;
                oldy = player->y;
                switch(c) {
                        case CMD_LEFT:
                                player->x--;
                                if(indungeon && hit_wall)
                                        player->x++;
                                if(player->x < 0)
                                        player->x = 0;
                                if(player->x <= (px+(mapcxsize/6)))
                                        px--;
                                if(px <= 0)
                                        px = 0;
                                mapchanged = 1;
                                break;
                        case CMD_LONGLEFT:
                                if(outside) {
                                        player->x -= 20;
                                        mapchanged = 1;
                                        if(player->x < 0)
                                                player->x = 0;
                                        if(player->x <= (px+(mapcxsize/6)))
                                                px-=20;
                                        if(px <= 0)
                                                px = 0;
                                }
                                break;
                        case CMD_RIGHT:
                                player->x++;
                                if(indungeon && hit_wall)
                                        player->x--;
                                if(player->x >= XSIZE-1)
                                        player->x = XSIZE-2;
                                if(player->x >= (px+(mapcxsize/6*5)))
                                        px++;
                                if(px >= XSIZE-mapcxsize)
                                        px = XSIZE-mapcxsize-1;
                                mapchanged = 1;
                                break;
                        case CMD_DOWN:
                                player->y++;
                                if(indungeon && hit_wall)
                                        player->y--;
                                if(player->y >= YSIZE-1)
                                        player->y = YSIZE-2;
                                if(player->y >= (py+(mapcysize/6*5)))
                                        py++;
                                if(py >= YSIZE-mapcysize)
                                        py = YSIZE-mapcysize-1;

                                mapchanged = 1;
                                break;
                        case CMD_UP:
                                player->y--;
                                if(indungeon && hit_wall)
                                        player->y++;
                                mapchanged=1;
                                if(player->y < 0) {
                                        player->y = 0;
                                        mapchanged=0;
                                }
                                if(player->y <= (py+(mapcysize/6)))
                                        py--;
                                if(py <= 0)
                                        py = 0;
                                break;
                        case CMD_LONGDOWN:
                                if(outside) {
                                        player->y+=20;
                                        mapchanged = 1;
                                        if(player->y >= YSIZE-1)
                                                player->y = YSIZE-2;
                                        if(player->y >= (py+(mapcysize/6*5)))
                                                py+=20;
                                        if(py >= YSIZE-mapcysize)
                                                py = YSIZE-mapcysize-1;
                                }
                                break;
                        case CMD_LONGUP:
                                if(outside) {
                                        player->y-=20;
                                        mapchanged = 1;
                                        if(player->y < 0)
                                                player->y = 0;
                                        if(player->y <= (py+(mapcysize/6)))
                                                py-=20;
                                        if(py <= 0)
                                                py = 0;
                                }
                                break;
                        case CMD_LONGRIGHT:
                                if(outside) {
                                        player->x += 20;
                                        mapchanged = 1;
                                        if(player->x >= XSIZE-1)
                                                player->x = XSIZE-2;
                                        if(player->x >= (px+(mapcxsize/6*5)))
                                                px+=20;
                                        if(px >= XSIZE-mapcxsize)
                                                px = XSIZE-mapcxsize-1;
                                }
                                break;
                        case CMD_TOGGLEFOV:
                                fov = !fov;
                                mapchanged=1;
                                break;

                        case CMD_QUIT:
                                goto clean_up;
                                break;

                        case CMD_SAVE:
                                save_world(world, player, "quicksave");
                                break;

                        case CMD_LOAD:
                                load_world(world, player, "quicksave");
                                px = player->x - (mapcxsize/2);
                                if(px < 0)
                                        px = 0;
                                py = player->y - (mapcysize/2);
                                if(py < 0)
                                        py = 0;
                                mapchanged=1;
                                break;
                        case CMD_LEAVEDUNGEON:
                                if(indungeon && has_exit(player->x, player->y)) {
                                        world->context = CONTEXT_OUTSIDE;
                                        loadpxy(player);
                                        px = player->x - (mapcxsize/2);
                                        if(px < 0)
                                                px = 0;
                                        py = player->y - (mapcysize/2);
                                        if(py < 0)
                                                py = 0;
                                        mapchanged = 1;
                                        fov = 0;
                                        switch_commandset(outsidecommands);
                                }
                                break;
                        case CMD_ENTERDUNGEON:
                                if(outside && has_dungeon(player->x, player->y)) {
                                        init_dungeon(world, mapcxsize, mapcysize);
                                        world->context = CONTEXT_DUNGEON;
                                        savepxy(player);
                                        get_dungeon_starting_location(player, world);
                                        world->dungeon.cell[player->y][player->x].flags |= HAS_EXIT;
                                        px = player->x - (mapcxsize/2);
                                        if(px < 0)
                                                px = 0;
                                        py = player->y - (mapcysize/2);
                                        if(py < 0)
                                                py = 0;
                                        world->current_city = world->current_village = world->current_forest = 0;
                                        init_fov_dungeon(world, player->fov);
                                        fov = 1;
                                        mapchanged = 1;
                                        switch_commandset(dungeoncommands);
                                }
                                break;
                        case CMD_PICKUP:
                                if(ccell.inventory) {
                                        if(ccell.inventory->type == OT_GOLD && ccell.inventory->quantity) {
                                                you_c(TCOD_green, "pick up %d pieces of gold.", ccell.inventory->quantity);
                                                seenothing = 0;
                                        } else {
                                                seenothing = 0;
                                        }
                                        if(movefromcelltoinventory(player, world))
                                                you_c(TCOD_red, "%s", WV_PICKUPNOTHING);

                                        cleanup_inventory(player);
                                } else {
                                        you_c(TCOD_red, "%s", WV_PICKUPNOTHING);
                                        seenothing = 0;
                                }
                                mapchanged = 1;
                                break;
                        case CMD_DROP:
                                if(player->inventory->next) {
                                        char dropwhat;
                                        dropwhat = askplayer("Drop which item?");
                                        if(dropwhat) {
                                                movefrominventorytocell(player, world, dropwhat);
                                        }
                                        cleanup_inventory(player);
                                } else {
                                        you_c(TCOD_red, "%s", WV_NOTHINGLEFT);
                                }
                                mapchanged = 1;
                                break;
                        case CMD_WIELD:
                                what = askplayer("Which item to (un)wield/wear?");
                                o = get_obj_by_letter(what);
                                wieldwear(o, player, 1);
                                mapchanged = 1;
                                seenothing = 0;
                                break;
                        case CMD_IDENTIFYALL:
                                o = player->inventory->next;
                                while(o!=NULL) {
                                        o->flags |= OF_IDENTIFIED;
                                        o = o->next;
                                }
                                mapchanged = 1;
                                break;
                        case CMD_SKILLSCREEN:
                                tmpconsole = TCOD_console_new(50,20);
                                TCOD_console_set_foreground_color(tmpconsole, TCOD_white);
                                TCOD_console_set_background_color(tmpconsole, TCOD_black);
                                TCOD_console_clear(tmpconsole);
                                showskills(&tmpconsole, player);
                                TCOD_console_blit(tmpconsole,0,0,50,20,NULL,40,10,1.0,1.0);
                                TCOD_console_flush();
                                TCOD_console_wait_for_keypress(false);
                                TCOD_console_delete(tmpconsole);
                                seenothing = 0;
                                mapchanged = 1;
                                break;
                        default:
                                break;
                }
/*
                if(key.vk == TCODK_ENTER && key.lalt)
                        TCOD_console_set_fullscreen(!TCOD_console_is_fullscreen());*/


                if(ccell.monster) {
                        ccell.monster->attacker = player;
                        attack(player, ccell.monster, world);
                        player->x = oldx;
                        player->y = oldy;
                        seenothing = 0;
                }

                move_monsters(world, player);

                if(mapchanged) {
                        if(fov) {
                                TCOD_map_set_properties(player->fov, player->x, player->y, true, true);
                                if(player->x < XSIZE-1) TCOD_map_set_properties(player->fov, player->x+1, player->y, true, true);
                                if(player->x) TCOD_map_set_properties(player->fov, player->x-1, player->y, true, true);
                                if(player->y < YSIZE-1) TCOD_map_set_properties(player->fov, player->x, player->y+1, true, true);
                                if(player->y) TCOD_map_set_properties(player->fov, player->x, player->y-1, true, true);
                                if(player->x < XSIZE-1 && player->y < YSIZE-1) TCOD_map_set_properties(player->fov, player->x+1, player->y+1, true, true);
                                if(player->y) TCOD_map_set_properties(player->fov, player->x+1, player->y-1, true, true);
                                if(player->x) TCOD_map_set_properties(player->fov, player->x-1, player->y+1, true, true);
                                if(player->x && player->y) TCOD_map_set_properties(player->fov, player->x-1, player->y-1, true, true);


                                if(outside)
                                        TCOD_map_compute_fov(player->fov, player->x, player->y, 5, true, FOV_BASIC);
                                if(indungeon)
                                        TCOD_map_compute_fov(player->fov, player->x, player->y, 2, true, FOV_RESTRICTIVE);
                        }

                        if(outside)
                                check_location(player, world);

                        if(fov) {
                                if(outside)
                                        draw_world_fov(px, py, player, mapcxsize, mapcysize, world, player->fov);
                                if(indungeon)
                                        draw_dungeon_fov(px, py, player, mapcxsize, mapcysize, world, player->fov);

                        } else {
                                if(indungeon)
                                        draw_dungeon(px, py, player, mapcxsize, mapcysize, world);
                                if(outside)
                                        draw_world(px, py, player, mapcxsize, mapcysize, world);
                        }

                        update_info(1, mapcysize+2, world, player, seenothing);

                        TCOD_console_set_foreground_color(NULL, TCOD_white);
                        TCOD_console_print_frame(NULL, mapcxsize+3, 0, (wmaxx-mapcxsize-3), 25, true, TCOD_BKGND_NONE, "You");
                        update_stats(mapcxsize+4, 1, world, player);
                        
                        TCOD_console_print_frame(NULL, 0, 0, mapcxsize+2, mapcysize+2, true, TCOD_BKGND_NONE, "Map");
                        TCOD_console_blit(map_console, 0, 0, mapcxsize, mapcysize, NULL, 1, 1, 1.0, 1.0);
                        TCOD_console_blit(mess_console, 0, 0, mapcxsize+2, MAX_MESS+2, NULL, 0, mapcysize+2, 1.0, 1.0);
                        TCOD_console_flush();
                        mapchanged = 0;
                }
        }
clean_up:

        TCOD_map_delete(player->fov);
        TCOD_random_delete(rndgen);
        free(player);
        free(world);
        return 0;
}
