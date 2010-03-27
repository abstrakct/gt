#ifndef DELVE_H
#define DELVE_H


int generate_dungeon(int xsize, int ysize, int ngb_min, int ngb_max, int connchance, int cellnum, world_t *world);
void init_dungeon(world_t *world, int maxxsize, int maxysize);

#endif
