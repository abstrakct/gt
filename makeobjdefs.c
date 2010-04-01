
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <libtcod.h>

#include "world.h"
#include "creature.h"
#include "objects.h"
//#include "gt.h"

extern int num_objects;
extern obj_t objects[];

#define INMAKEOBJDEFS

int main()
{
        FILE *f;
        char s[250];
        int i;

        f = fopen("objdefines.h", "w");
        fprintf(f, "#ifndef OBJDEFINES_H\n#define OBJDEFINES_H\n\n");

        init_objects();

        for(i=0;i<num_objects;i++) {
                int j;
                strcpy(s, get_def_name(objects[i]));
                for(j=0;j<strlen(s);j++) {
                        if(s[j] == ' ')
                                s[j] = '_';

                        if(s[j] == '+')
                                s[j] = 'P';
                        
                        if(s[j] == '-') {
                                if(s[j+1] >= '0' && s[j+1] <= '9')
                                        s[j] = 'M';
                                else
                                        s[j] = '_';
                        }


                        if(s[j] >= 'a' && s[j] <= 'z')
                                s[j] -= 32;
                }
                if(!strncmp(s, "FIRST", 5))
                        fprintf(f, "#define %s %d\n", s, i+1);
                else if(!strncmp(s, "LAST", 4))
                        fprintf(f, "#define %s %d\n", s, i-1);
                else
                        fprintf(f, "#define %s %d\n", s, i);
        }

        fprintf(f, "\n#define NUM_OBJECTS %d\n", i);

        fprintf(f, "\n#endif\n");
        fclose(f);

        return 0;
}


