
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libtcod.h>
#include <stdint.h>

#include "objects.h"
#include "gt.h"
#include "world.h"
#include "creature.h"
#include "objdefines.h"

char *matstr[] = { 0, "golden", "silver", "bronze", "wooden", "iron", "copper", "marble", "glass", "bone", "platinum", "steel", "blackwood", "brass", "ebony" };
extern obj_t objects[];

void init_materials()
{
        int i, j, count, mat;
        srand(getpid());

        mat = 1;
        count = 0;
        j = FIRST_WAND + (rand() % (LAST_WAND - FIRST_WAND + 1));
        while(count <= (LAST_WAND-FIRST_WAND)) {
                if(objects[j].material == 0) {
                        objects[j].material = mat;
                        mat++;
                        count++;
                }
                j = FIRST_WAND + (rand() % (LAST_WAND - FIRST_WAND+1));
        }
        
        for(i=FIRST_WAND;i<=LAST_WAND;i++) {
                char tmp[100];
                strcpy(tmp, objects[i].unidname);
                sprintf(objects[i].unidname, "%s %s", matstr[(int)objects[i].material], tmp);
                printf("%s is a %s\n", objects[i].fullname, objects[i].unidname);
        }

        mat = 1;
        count = 0;
        j = FIRST_RING + (rand() % (LAST_RING - FIRST_RING + 1));
        while(count <= (LAST_RING - FIRST_RING)) {
                if(objects[j].material == 0) {
                        objects[j].material = mat;
                        mat++;
                        count++;
                }
                j = FIRST_RING + (rand() % (LAST_RING - FIRST_RING + 1));
        }

        for(i=FIRST_RING;i<=LAST_RING;i++) {
                char tmp[100];
                strcpy(tmp, objects[i].unidname);
                sprintf(objects[i].unidname, "%s %s", matstr[(int)objects[i].material], tmp);
                printf("%s is a %s\n", objects[i].fullname, objects[i].unidname);
        }
}
