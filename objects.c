/*
 *
 * Dealing wit objects.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libtcod.h>

#include "objects.h"
#include "gt.h"
#include "world.h"
#include "creature.h"
#include "objlist.h"

char objchars[] = { ')', '[', '=', '#', '/', '%', '$' };
int num_objects;

void uppercase(char *s)
{
        int i;

        for(i=0;i<strlen(s);i++) {
                if(s[i] >= 97 && s[i] <= 122) {
                        s[i] -= 32;
                        return;
                }
        }
}

char *a_an(char *s)
{
        static char ret[4];

        if(s[0] == 'a' || s[0] == 'A' ||
           s[0] == 'e' || s[0] == 'A' ||
           s[0] == 'i' || s[0] == 'I' ||
           s[0] == 'o' || s[0] == 'O' ||
           s[0] == 'u' || s[0] == 'U' ||
           s[0] == 'y' || s[0] == 'Y')
                strcpy(ret, "an");
        else
                strcpy(ret, "a");

        return ret;
}


char *get_def_name(obj_t object)
{
        static char name[250];
        char tmp[5];

        name[0] = '\0';

        /*if(is_eatable(object))
                strcat(name, "eatable ");

        if(is_drinkable(object))
                strcat(name, "drinkable ");*/

        if(is_magic(object.flags))
                if(object.modifier) {
                        if(object.modifier > 0)
                                strcat(name, "+");
                        sprintf(tmp, "%d ", object.modifier);
                        strcat(name, tmp);
                }

        strcat(name, object.basename);

        if(is_holyfuck(object.flags))
                strcat(name, " of Holy Fuck");

        uppercase(name);
        return name;
}

char *get_full_name(obj_t object)
{
        static char name[250];
        static char tmpname[250];
        static char prefix[5];
        char tmp[5];

        name[0] = '\0';

        /*if(is_eatable(object))
                strcat(name, "eatable ");

        if(is_drinkable(object))
                strcat(name, "drinkable ");*/

        if(is_magic(object.flags))
                if(object.modifier) {
                        if(object.modifier > 0)
                                strcat(name, "+");
                        sprintf(tmp, "%d ", object.modifier);
                        strcat(name, tmp);
                }

        strcat(name, object.basename);
        strcpy(prefix, a_an(name));

        if(is_holyfuck(object.flags))
                strcat(name, " of Holy Fuck!");

        if(object.unique)
                strcpy(prefix, "the");

        strcpy(tmpname, name);
        sprintf(name, "%s %s", prefix, tmpname);
        uppercase(name);
        return name;
}

void moveobject(obj_t *src, obj_t *dest)
{
        obj_t *tmp;
       // obj_t *p;

        tmp = dest;
        while(tmp->next != NULL)
                tmp = tmp->next;

//        tmp->prev = get_last_object(tmp);
        
        if(src->prev)
                src->prev->next = src->next;
        else
                src->next = NULL;

        tmp->next = src;
        tmp->next->next = NULL;
        tmp->next->prev = tmp->prev->next;


        src = NULL;
}


int wieldable(obj_t *obj)
{
        if(obj->type == OT_WEAPON)
                return 1;
        else
                return 0;
}

int wearable(obj_t *obj)
{
        if(obj->type == OT_ARMOR)
               return 1;
        else
               return 0;
} 


void init_objects()
{
        int i;

        num_objects = 0;
        for(i=0;i<(sizeof(objects)/sizeof(obj_t));i++) {
                num_objects++;
                if(!(objects[i].flags & OF_DONOTUSE)) {
                        strcpy(objects[i].fullname, get_full_name(objects[i]));
                }
                //printf("objects[%d] = %s\n", i, objects[i].fullname);
        }
}
