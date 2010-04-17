/* Written by Kusigrosz in December 2008
 * This program is in public domain, with all its bugs etc.
 * No warranty whatsoever.
 *
 * Adapted to work for GT by Rolf Klausen 2010.
 *
 * Delve a random maze/cavern. The results are written as text to stdout
 * and as a bmp file to BMP_FILENAME (see define below),
 *
 * Usage:
 *     a.out ngb_min ngb_max connchance cellnum
 * Arguments:
 *     ngb_min, ngb_max: the minimum and maximum number of neighbouring
 *         floor cells that a wall cell must have to become a floor cell.
 *         1 <= ngb_min <= 3; ngb_min <= ngb_max <= 8;
 *     connchance: the chance (in percent) that a new connection is
 *         allowed; for ngb_max == 1 this has no effect as any
 *         connecting cell must have 2 neighbours anyway.
 *     cellnum: the maximum number of floor cells that will be generated.
 * The default values of the arguments are defined below.
 *
 * Algorithm description:
 * The algorithm operates on a rectangular grid. Each cell can be 'wall'
 * or 'floor'. A (non-border) cell has 8 neigbours - diagonals count. 
 * There is also a cell store with two operations: store a given cell on
 * top, and pull a cell from the store. The cell to be pulled is selected
 * randomly from the store if N_cells_in_store < 125, and from the top
 * 25 * cube_root(N_cells_in_store) otherwise. There is no check for
 * repetitions, so a given cell can be stored multiple times.
 *
 * The algorithm starts with most of the map filled with 'wall', with a
 * "seed" of some floor cells; their neigbouring wall cells are in store.
 * The main loop in delveon() is repeated until the desired number of 
 * floor cells is achieved, or there is nothing in store:
 *     1) Get a cell from the store;
 *     Check the conditions: 
 *     a) the cell has between ngb_min and ngb_max floor neighbours, 
 *     b) making it a floor cell won't open new connections,
 *         or the RNG allows it with connchance/100 chance.
 *     if a) and b) are met, the cell becomes floor, and its wall
 *     neighbours are put in store in random order.
 * There are many variants possible, for example:
 * 1) picking the cell in rndpull() always from the whole store makes
 *     compact patterns;
 * 2) storing the neighbours in digcell() clockwise starting from
 *     a random one, and picking the bottom cell in rndpull() creates
 *     meandering or spiral patterns.
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <libtcod.h>

#include "gt.h"
#include "world.h"

#define FLOOR   ' '
#define WALL    '#'

#define RQR(p) {if (!(p)) \
    {fprintf(stderr, "requirement failed: line %d\n", __LINE__); \
    exit(EXIT_FAILURE);}}

struct mappiece {
    char** map;
    int xsize;
    int ysize;
};

struct cellstore {
    int* xs;
    int* ys;
    int index;
    int size;
};

/* Declarations of all functions */
struct mappiece mpc_mkempty(int xsize, int ysize);
int rnd_i0(int n);
void rnd_perm(int *tab, int nelem);
int uti_icbrt(int arg);
int inbord(struct mappiece mpc, int x, int y);
struct cellstore mkstore(int size);
void delstore(struct cellstore* cstore);
int storecell(struct cellstore* cstore, int x, int y);
int rndpull(struct cellstore* cstore, int* x, int* y);
int ngbcount(struct mappiece mpc, int x, int y, char trn);
int ngbgroups(struct mappiece mpc, int x, int y, char trn);
int digcell(struct mappiece mpc, struct cellstore* cstore,
    int x, int y, char flo, char ava);
int delveon(struct mappiece mpc, struct cellstore* cstore,
    int ngb_min, int ngb_max, int connchance, int cellnum,
    char flo, char ava);
int cavern(struct mappiece mpc, int xorig, int yorig,
    int ngb_min, int ngb_max, int connchance, int cellnum,
    char flo, char ava);
int cellnum_est(int totalcells, int ngb_min, int ngb_max);
int main(int argc, char* argv[]);


/* Globals: */
int Xoff[8] = {1,  1,  0, -1, -1, -1,  0,  1};
int Yoff[8] = {0,  1,  1,  1,  0, -1, -1, -1};

/* Functions */

struct mappiece mpc_mkempty(int xsize, int ysize)
{
        int x;
        struct mappiece mpc;

        RQR((xsize > 0) && (ysize > 0));

        mpc.map = malloc(xsize * sizeof(char*));
        RQR(mpc.map);
        for (x = 0; x < xsize; x++) {
                mpc.map[x] = malloc(ysize * sizeof(char));
                RQR(mpc.map[x]);
        }
        mpc.xsize = xsize;
        mpc.ysize = ysize;
        return (mpc);
}

int rnd_i0(int n) /* 0 <= rnd_i0(n) < n */
{
        RQR(n > 0);
//        return ((int) (rand() % n)); /* not very good */

        return ((int) (ri(0, n-1)));
}

void rnd_perm(int *tab, int nelem)
{
        int i;
        int rind;
        int tmp;

        RQR(tab && (nelem >= 0));

        for (i = 0; i < nelem; i++) {
                rind = rnd_i0(i + 1);

                tmp = tab[rind];
                tab[rind] = tab[i];
                tab[i] = tmp;
        }
}

/* Integer cube root approximation (from below) for positive ints < 2**31
 */
int uti_icbrt(int arg)
{
        int delta;
        int ret;

        RQR((arg >= 0) && (arg <= 0x7fffffff));

        if (arg == 0)
                return (0);

        if (arg < 1000)
                ret = 10;
        else if (arg < 1000000)
                ret = 100;
        else if (arg < 1000000000)
                ret = 1000;
        else
                ret = 1290; /* the largest x for which x**3 < 2**31 */

        do {
                /* derivative = 3 * x * x yadda yadda, but this seems to works OK */
                delta = (arg - ret * ret * ret) / (2 * ret * ret);
                ret += delta;
        } while (delta);

        if (ret * ret * ret > arg)
                ret--;

        return (ret);
}

/* 
 * Is the location within the borders - with 1 cell margin
 */
int inbord(struct mappiece mpc, int x, int y)
{
        if ((x < 1) || (x >= mpc.xsize - 1) || 
                        (y < 1) || (y >= mpc.ysize - 1)) {
                return (0);
        }
        return (1);
}

struct cellstore mkstore(int size)
{
        struct cellstore ret;

        RQR(size > 0);

        ret.xs = malloc(size * sizeof(int));
        RQR(ret.xs);
        ret.ys = malloc(size * sizeof(int));
        RQR(ret.ys);

        ret.size = size;
        ret.index = 0;

        return (ret);
}

void delstore(struct cellstore* cstore)
{
        RQR(cstore);

        free(cstore->xs);
        cstore->xs = 0;

        free(cstore->ys);
        cstore->ys = 0;

        cstore->size = 0;
        cstore->index = 0;
}

int storecell(struct cellstore* cstore, int x, int y)
{
        int rind;

        RQR(cstore);

        if (cstore->index < cstore->size) {
                cstore->xs[cstore->index] = x;
                cstore->ys[cstore->index] = y;
                (cstore->index)++;
                return (1); /* new cell stored */
        } else { /* Replace another cell. Should not happen if lossless storage */
                rind = rnd_i0(cstore->index);
                cstore->xs[rind] = x;
                cstore->ys[rind] = y;
                return (0); /* old cell gone, new cell stored */
        }
}

/* Remove a cell from the store and put its coords into x, y.
 * Note that pulling any cell except the topmost puts the topmost one in
 * its place. 
 */
int rndpull(struct cellstore* cstore, int* x, int* y)
{
        int rind;

        RQR(cstore && x && y);
        if (cstore->index <= 0)
                return(0); /* no cells */

        /* compact patterns */
        /* rind = rnd_i0(cstore->index); */

        /* fluffy patterns */
        rind = (cstore->index < 125) ?
                rnd_i0(cstore->index) :
                cstore->index - rnd_i0(25 * uti_icbrt(cstore->index)) - 1;

        *x = cstore->xs[rind];
        *y = cstore->ys[rind];

        if (cstore->index - 1 != rind) { /* not the topmost cell - overwrite */
                cstore->xs[rind] = cstore->xs[cstore->index - 1];
                cstore->ys[rind] = cstore->ys[cstore->index - 1];
        }

        cstore->index -= 1;

        return(1);
}

/* Count neighbours of the given cells that contain terrain trn.
 */
int ngbcount(struct mappiece mpc, int x, int y, char trn)
{
        int i;
        int count = 0;

        RQR(mpc.map);

        for (i = 0; i < 8; i++) {
                if (inbord(mpc, x + Xoff[i], y + Yoff[i]) && (mpc.map[x + Xoff[i]][y + Yoff[i]] == trn)) {
                        count++;
                }
        }
        return (count);
}

/* Number of groups of '1's in the 8 neighbours around a central cell.
 * The encoding is binary, lsb is to the right, then clockwise.
 */
const int ngb_grouptab[256] = 
    {
/**********  0  1  2  3  4  5  6  7  8  9  */
/* 000 */    0, 1, 1, 1, 1, 1, 1, 1, 1, 2,
/* 010 */    2, 2, 1, 1, 1, 1, 1, 2, 2, 2,
/* 020 */    1, 1, 1, 1, 1, 2, 2, 2, 1, 1,
/* 030 */    1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
/* 040 */    2, 3, 3, 3, 2, 2, 2, 2, 1, 2,
/* 050 */    2, 2, 1, 1, 1, 1, 1, 2, 2, 2,
/* 060 */    1, 1, 1, 1, 1, 1, 2, 1, 2, 1,
/* 070 */    2, 1, 2, 2, 3, 2, 2, 1, 2, 1,
/* 080 */    1, 1, 2, 1, 1, 1, 1, 1, 1, 1,
/* 090 */    2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
/* 000 */    2, 1, 2, 1, 2, 2, 3, 2, 2, 1,
/* 110 */    2, 1, 1, 1, 2, 1, 1, 1, 1, 1,
/* 120 */    1, 1, 2, 1, 1, 1, 1, 1, 1, 1,
/* 130 */    2, 1, 2, 1, 2, 1, 2, 2, 3, 2,
/* 140 */    2, 1, 2, 1, 2, 2, 3, 2, 2, 1,
/* 150 */    2, 1, 2, 2, 3, 2, 2, 1, 2, 1,
/* 160 */    2, 2, 3, 2, 3, 2, 3, 2, 3, 3,
/* 170 */    4, 3, 3, 2, 3, 2, 2, 2, 3, 2,
/* 180 */    2, 1, 2, 1, 2, 2, 3, 2, 2, 1,
/* 190 */    2, 1, 1, 1, 2, 1, 2, 1, 2, 1,
/* 200 */    2, 2, 3, 2, 2, 1, 2, 1, 1, 1,
/* 210 */    2, 1, 1, 1, 1, 1, 1, 1, 2, 1,
/* 220 */    1, 1, 1, 1, 1, 1, 2, 1, 2, 1,
/* 230 */    2, 1, 2, 2, 3, 2, 2, 1, 2, 1,
/* 240 */    1, 1, 2, 1, 1, 1, 1, 1, 1, 1,
/* 250 */    2, 1, 1, 1, 1, 1
    };

/* Examine the 8 neigbours of the given cell, and count the number
 * of separate groups of trn cells. A groups contains cells that are
 * of the same type (trn) and are adjacent (diagonally, too!)
 */
int ngbgroups(struct mappiece mpc, int x, int y, char trn)
{
        int bitmap = 0; /* lowest bit is the cell to the right, then clockwise */
        int i;

        RQR(mpc.map);

        for (i = 0; i < 8; i++) {
                bitmap >>= 1;

                if (inbord(mpc, x + Xoff[i], y + Yoff[i]) && mpc.map[x + Xoff[i]][y + Yoff[i]] == trn) {
                        bitmap |= 0x80;
                }
        }

        return (ngb_grouptab[bitmap]);
}

/* Dig out an ava cell to flo and store its ava neighbours in
 * random order.
 */
int digcell(struct mappiece mpc, struct cellstore* cstore, int x, int y, char flo, char ava)
{
        int order[8] = {0, 1, 2, 3, 4, 5, 6, 7};
        int i, j;

        RQR(cstore && mpc.map);

        if ((!inbord(mpc, x, y)) || (mpc.map[x][y] != ava))
                return (0); /* did nothing */

        mpc.map[x][y] = flo;

        rnd_perm(order, 8);

        for (i = 0; i < 8; i++) {
                j = order[i];
                if (inbord(mpc, x + Xoff[j], y + Yoff[j]) && (mpc.map[x + Xoff[j]][y + Yoff[j]] == ava)) {
                        storecell(cstore, x + Xoff[j], y + Yoff[j]);
                }
        }

        return (1); /* dug 1 cell */
}

/* Continue digging until cellnum or no more cells in store. Digging is
 * allowed if the terrain in the cell is 'ava'ilable, cell has from
 * ngb_min to ngb_max flo neighbours, and digging won't open new
 * connections; the last condition is ignored with percent chance
 * connchance.
 */
int delveon(struct mappiece mpc, struct cellstore* cstore,
                int ngb_min, int ngb_max, int connchance, int cellnum,
                char flo, char ava)
{
        int count = 0;
        int ngb_count;
        int ngb_groups;
        int x, y;

        RQR(cstore && mpc.map);
        RQR((cellnum >= 0) && (cellnum < mpc.xsize * mpc.ysize));
        RQR((connchance >= 0) && (connchance <= 100));
        RQR((ngb_min >= 0) && (ngb_min <= 3) && 
                        (ngb_min <= ngb_max) && (ngb_max <= 8));
        RQR(flo != ava);

        while ((count < cellnum) && rndpull(cstore, &x, &y)) {
                ngb_count = ngbcount(mpc, x, y, flo);
                ngb_groups = ngbgroups(mpc, x, y, flo);

                if ( inbord(mpc, x, y) && (mpc.map[x][y] == ava) &&
                                (ngb_count >= ngb_min) && (ngb_count <= ngb_max) && 
                                ((ngb_groups <= 1) || (rnd_i0(100) < connchance)) ) {
                        count += digcell(mpc, cstore, x, y, flo, ava);
                }
        }

        return (count);
}

/* Generate a random cavern of cellnum cells. 
 */
int cavern(struct mappiece mpc, int xorig, int yorig,
                int ngb_min, int ngb_max, int connchance, int cellnum, 
                char flo, char ava)
{
        struct cellstore cstore;
        int count = 0;
        int ngb_count;
        int ngb_groups;
        int x, y;

        RQR(mpc.map);
        RQR((cellnum >= 0) && (cellnum < mpc.xsize * mpc.ysize));
        RQR((connchance >= 0) && (connchance <= 100));
        RQR((ngb_min >= 0) && (ngb_min <= 3) && 
                        (ngb_min <= ngb_max) && (ngb_max <= 8));
        RQR(flo != ava);

        cstore = mkstore(8 * mpc.xsize * mpc.ysize); /* overkill */
        storecell(&cstore, xorig, yorig);

        while ((count < 2 * ngb_min) && (count < cellnum) && rndpull(&cstore, &x, &y)) {
                ngb_count = ngbcount(mpc, x, y, flo);
                ngb_groups = ngbgroups(mpc, x, y, flo);

                /* stay close to origin, ignore ngb_min */
                if ( inbord(mpc, x, y) && (mpc.map[x][y] == ava) &&
                                (abs(x - xorig) < 2) && (abs(y - yorig) < 2) &&
                                (ngb_count <= ngb_max) && 
                                ((ngb_groups <= 1) || (rnd_i0(100) < connchance)) ) {
                        count += digcell(mpc, &cstore, x, y, flo, ava);
                }
        }

        if (count < cellnum) {
                count += delveon(mpc, &cstore, ngb_min, ngb_max, connchance, 
                                cellnum - count, flo, ava);
        }

        delstore(&cstore);
        return (count);
}

/* Estimate a sensible number of cells for given ngb_min, ngb_max.
 */
int cellnum_est(int totalcells, int ngb_min, int ngb_max)
{
        int denom[12] = {8, 8, 8, 7, 6, 5, 5, 4, 4, 4, 3, 3};
        /* two first entries are not used */

        RQR(totalcells > 0);
        RQR((ngb_min + ngb_max >= 2) && (ngb_min + ngb_max < 12));

        return (totalcells / denom[ngb_min + ngb_max]);
}

//int main(int argc, char* argv[])
int generate_dungeon(int xsize, int ysize, int ngb_min, int ngb_max, int connchance, int cellnum, world_t *world)
{
        struct mappiece mpc;
        int arg_int[10]; /* arg_int[0] is dummy */
        int x, y;
        int dugcells = 0;
        int i;

        mpc = mpc_mkempty(xsize, ysize);

        //srand((unsigned int)time(NULL));

        for (x = 0; x < mpc.xsize; x++) {
                for (y = 0; y < mpc.ysize; y++) {
                        mpc.map[x][y] = WALL;
                }
        }

        for (i = 0; i < 10; i++)
        {
                arg_int[i] = 0; /* initiate to something */
        }

        /* default arguments */
        arg_int[1] = ngb_min;
        arg_int[2] = ngb_max;
        arg_int[3] = connchance;
        arg_int[4] = 1;               /* cellnum, will be overwritten below */

        if ((arg_int[1] < 1) || (arg_int[1] > 3) || 
                        (arg_int[1] > arg_int[2]) || (arg_int[2] > 8) || 
                        (arg_int[3] < 0) || (arg_int[3] > 100) ||
                        (arg_int[4] <= 0) || (arg_int[4] >= mpc.xsize * mpc.ysize)) {
                return 1;
        }
        /* cellnum: if user provided no value, guess something sensible */
        if (!cellnum) {
                arg_int[4] = cellnum_est(mpc.xsize * mpc.ysize, 
                                arg_int[1], arg_int[2]);
        }

        dugcells = cavern(mpc, mpc.xsize/2, mpc.ysize/2, 
                        arg_int[1], arg_int[2], arg_int[3], arg_int[4], FLOOR, WALL);

        for (y = 0; y < mpc.ysize; y++) {
                for (x = 0; x < mpc.xsize; x++) {
                        //printf("%c", mpc.map[x][y]);
                        if(mpc.map[x][y] == WALL)
                                world->dungeon.cell[y][x].type = D_WALL;
                        if(mpc.map[x][y] == FLOOR)
                                world->dungeon.cell[y][x].type = D_FLOOR;
                        world->dungeon.cell[y][x].color = TCOD_white;
                }
        }

        world->dungeon.xsize = mpc.xsize;
        world->dungeon.ysize = mpc.ysize;
        printf("generated dungeon size %d x %d\n", world->dungeon.xsize, world->dungeon.ysize);
        return 0;
}

void nothingtowall(world_t *world, int x, int y)
{
        if(world->dungeon.cell[y][x].type == D_NOTHING)
                world->dungeon.cell[y][x].type = D_WALL;
}

void set_floor(world_t *world, int x, int y)
{
        world->dungeon.cell[y][x].type = D_FLOOR;
        // whatever nothing-cells are around the floor is set to wall
        nothingtowall(world, x-1, y);
        nothingtowall(world, x, y-1);
        nothingtowall(world, x, y+1);
        nothingtowall(world, x+1, y);
        nothingtowall(world, x-1, y-1);
        nothingtowall(world, x-1, y+1);
        nothingtowall(world, x+1, y-1);
        nothingtowall(world, x+1, y+1);
}

void make_corridor(world_t *world, TCOD_map_t *map, int x, int y, int x2, int y2)
{
        TCOD_path_t path;
        int i, a, b;

        path = TCOD_path_new_using_map(*map, 1.41f);
        TCOD_path_compute(path, x, y, x2, y2);
        if(!path)
                die("no path!");

        while(!TCOD_path_is_empty(path)) {
                int x, y;
                if(TCOD_path_walk(path, &x, &y, false)) {
//                        world->dungeon.cell[y][x].type = D_FLOOR;
                        set_floor(world, x, y);
                        i = ri(1,100);
                        if(i<=70) {
                                a = ri(-1,1);
                                b = ri(-1,1);
                                // add some more randomness!
                                set_floor(world, x+b, y+a);
                        }
                }
        }

        TCOD_path_delete(path);
}

void make_room_left(world_t *world, int x, int y, int w, int h)
{
        int sx, sy;

        // let's try to make it simple, stupid
        for(sx = x; sx>=(x-w); sx--) {
                for(sy = y; sy>=(y-h); sy--) {
                        set_floor(world, sx, sy);
                        //world->dungeon.cell[sy][sx].type = D_FLOOR;
                }
        }
}

void make_room_right(world_t *world, int x, int y, int w, int h)
{
        int sx, sy;

        // let's try to make it simple, stupid
        for(sx = x; sx<=(x+w); sx++) {
                for(sy = y; sy<=(y+h); sy++) {
                        set_floor(world, sx, sy);
                        //world->dungeon.cell[sy][sx].type = D_FLOOR;
                }
        }
}

void make_room_down(world_t *world, int x, int y, int w, int h)
{
        int sx, sy;

        // let's try to make it simple, stupid
        for(sx = x; sx<=(x+w); sx++) {
                for(sy = y; sy<=(y+h); sy++) {
                        set_floor(world, sx, sy);
                        //world->dungeon.cell[sy][sx].type = D_FLOOR;
                }
        }
}

void clean_up_dungeon(world_t *world)
{
        int x, y;

        x = y = 0;
        for(x=0;x</*world->dungeon.xsize*/XSIZE;x++) {
                for(y=0;y</*world->dungeon.ysize*/YSIZE;y++) {
                        if(world->dungeon.cell[y][x].type == D_FLOOR && world->dungeon.cell[y][x+1].type == D_WALL && world->dungeon.cell[y][x+2].type == D_FLOOR) {
                                world->dungeon.cell[y][x+1].type = D_FLOOR;
                        }

                        if(world->dungeon.cell[y][x].type == D_FLOOR && world->dungeon.cell[y+1][x].type == D_WALL && world->dungeon.cell[y+2][x].type == D_FLOOR) {
                                world->dungeon.cell[y+1][x].type = D_FLOOR;
                        }
                }
        }
}

void my_generate_dungeon(world_t *world, int maxx, int maxy)
{
        // OK, first attempt at writing my own dungeon generator!

        TCOD_map_t map;
        int i, j, x, y, x2, y2, w, h, dir, counter;
        int startleftx, startlefty, startrightx, startrighty;

        world->dungeon.xsize = maxx;
        world->dungeon.ysize = maxy;
        map = TCOD_map_new(XSIZE, YSIZE);

        // First, set everything to be nothing 
        for(i=0;i<XSIZE;i++) {
                for(j=0;j<YSIZE;j++) {
                        world->dungeon.cell[j][i].flags = 0;
                        world->dungeon.cell[j][i].type = D_NOTHING;
                        world->dungeon.cell[j][i].color = TCOD_white;
                        TCOD_map_set_properties(map, i, j, true, true);
                }
        }

        x = ri(0,maxx);
        y = ri(0,maxy);
        x2 = x + ri(0,20);
        y2 = y + ri(0,20);

        // Then, make a corridor
        make_corridor(world, &map, x, y, x2, y2);
        
        // a room at each end of the corridor...
        w = ri(3, 10);
        h = ri(3, 10);
        make_room_left(world, x, y, w, h);
        w = ri(3, 10);
        h = ri(3, 10);
        make_room_right(world, x2, y2, w, h);

        /* ok, we have a starting point. Now, lets expand. */
        startleftx = x; startlefty = y; startrightx = x2; startrighty = y2;
        counter = 0;

        while(counter <= maxy/10) {
                for(i=0;i<XSIZE;i++) {
                        for(j=0;j<YSIZE;j++) {
                                if(world->dungeon.cell[j][i].type == D_WALL)
                                        TCOD_map_set_properties(map, i, j, true, false);
                        }
                }
                x += w;
                y += h;
                dir = ri(0,1);
                if(dir) { // right 
                        y += ri((0-h),h);
                } else {
                        x += ri((0-w),w);
                }
                if(dir) {
                        y2 = y + ri(h+5,40);
                } else {
                        x2 = x - ri(w+5,40);
                }

                make_corridor(world, &map, x, y, x2, y2);
                //x = x2;
                //y = y2;
                w = ri(3, 10);
                h = ri(3, 10);
                if(dir) { // y
                        make_room_right(world, x2, y2, w, h);
                        counter++;
                } else {
                        make_room_left(world, x2, y2, w, h);
                        counter++;
                }

                x2 = x;
                y2 = y;
                clean_up_dungeon(world);
                clean_up_dungeon(world);
                clean_up_dungeon(world);
        }


}

void init_dungeon(world_t *world, int maxxsize, int maxysize)
{
        int d;
        extern int mapcxsize, mapcysize;

        d = ri(0,99);
        world->dungeon.xsize = maxxsize + ri(0, (800 - mapcxsize)-1);
        world->dungeon.ysize = maxysize + ri(0, (800 - mapcysize)-1);


        if(d>=0) {
                my_generate_dungeon(world, 125, 80);
                return;
        }

        if(d <= 33)
                if(generate_dungeon(world->dungeon.xsize, world->dungeon.ysize, 3, 8, 0, 0, world))
                        die("Dungeon generation failed!");
        if(d > 33 && d <= 66)
                if(generate_dungeon(world->dungeon.xsize, world->dungeon.ysize, 2, 3, 0, 0, world))
                        die("Dungeon generation failed!");
        if(d > 66)
                if(generate_dungeon(world->dungeon.xsize, world->dungeon.ysize, 1, 8, 0, 0, world))
                        die("Dungeon generation failed!");
}
