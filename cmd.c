/*
 *
 * Handling commands (keyboard input)
 *
 */

#include <stdio.h>
#include <libtcod.h>
#include <string.h>

#include "cmd.h"


cmd_t outsidecommands[] = {
        { { TCODK_DOWN,     0, 1, 0, 0, 0, 0, 0 }, CMD_DOWN, "Move down" },
        { { TCODK_KP2,      0, 1, 0, 0, 0, 0, 0 }, CMD_DOWN, "Move down" },
        { { TCODK_UP,       0, 1, 0, 0, 0, 0, 0 }, CMD_UP, "Move up" },
        { { TCODK_KP8,      0, 1, 0, 0, 0, 0, 0 }, CMD_UP, "Move up" },
        { { TCODK_LEFT,     0, 1, 0, 0, 0, 0, 0 }, CMD_LEFT, "Move left" },
        { { TCODK_KP4,      0, 1, 0, 0, 0, 0, 0 }, CMD_LEFT, "Move left" },
        { { TCODK_RIGHT,    0, 1, 0, 0, 0, 0, 0 }, CMD_RIGHT, "Move right" },
        { { TCODK_KP6,      0, 1, 0, 0, 0, 0, 0 }, CMD_RIGHT, "Move right" },
        { { TCODK_CHAR,    44, 1, 0, 0, 0, 0, 0 }, CMD_PICKUP, "Pick up something" },
        { { TCODK_CHAR,   'd', 1, 0, 0, 0, 0, 0 }, CMD_DROP, "Drop something" },
        { { TCODK_ESCAPE,   0, 1, 0, 0, 0, 0, 0 }, CMD_QUIT, "Quit" },
        { { TCODK_CHAR,   'q', 1, 0, 0, 0, 0, 0 }, CMD_QUIT, "Quit" },
        { { TCODK_PAGEUP,   0, 1, 0, 0, 0, 0, 0 }, CMD_LONGUP, "" },
        { { TCODK_PAGEDOWN, 0, 1, 0, 0, 0, 0, 0 }, CMD_LONGDOWN, "" },
        { { TCODK_HOME,     0, 1, 0, 0, 0, 0, 0 }, CMD_LONGLEFT, "" },
        { { TCODK_END,      0, 1, 0, 0, 0, 0, 0 }, CMD_LONGRIGHT, "" },
        { { TCODK_CHAR,   'f', 1, 0, 0, 0, 0, 0 }, CMD_TOGGLEFOV, "Toggle FOV" },
        { { TCODK_F5,       0, 1, 0, 0, 0, 0, 0 }, CMD_SAVE, "Save" },
        { { TCODK_F6,       0, 1, 0, 0, 0, 0, 0 }, CMD_LOAD, "Load" },
        { { TCODK_CHAR,   'e', 1, 0, 0, 0, 0, 0 }, CMD_ENTERDUNGEON, "Enter dungeon" },
        { { TCODK_CHAR,   'w', 1, 0, 0, 0, 0, 0 }, CMD_WIELD, "Wield/wear" },
};

cmd_t dungeoncommands[] = {
        { { TCODK_DOWN,     0, 1, 0, 0, 0, 0, 0 }, CMD_DOWN, "Move down" },
        { { TCODK_KP2,      0, 1, 0, 0, 0, 0, 0 }, CMD_DOWN, "Move down" },
        { { TCODK_UP,       0, 1, 0, 0, 0, 0, 0 }, CMD_UP, "Move up" },
        { { TCODK_KP8,      0, 1, 0, 0, 0, 0, 0 }, CMD_UP, "Move up" },
        { { TCODK_LEFT,     0, 1, 0, 0, 0, 0, 0 }, CMD_LEFT, "Move left" },
        { { TCODK_KP4,      0, 1, 0, 0, 0, 0, 0 }, CMD_LEFT, "Move left" },
        { { TCODK_RIGHT,    0, 1, 0, 0, 0, 0, 0 }, CMD_RIGHT, "Move right" },
        { { TCODK_KP6,      0, 1, 0, 0, 0, 0, 0 }, CMD_RIGHT, "Move right" },
        { { TCODK_CHAR,   'w', 1, 0, 0, 0, 0, 0 }, CMD_WIELD, "Wield/wear" },
        { { TCODK_CHAR,    44, 1, 0, 0, 0, 0, 0 }, CMD_PICKUP, "Pick up something" },
        { { TCODK_CHAR,   'd', 1, 0, 0, 0, 0, 0 }, CMD_DROP, "Drop something" },
        { { TCODK_ESCAPE,   0, 1, 0, 0, 0, 0, 0 }, CMD_QUIT, "Quit" },
        { { TCODK_CHAR,   'q', 1, 0, 0, 0, 0, 0 }, CMD_QUIT, "Quit" },
        { { TCODK_CHAR,   'f', 1, 0, 0, 0, 0, 0 }, CMD_TOGGLEFOV, "Toggle FOV" },
        { { TCODK_F5,       0, 1, 0, 0, 0, 0, 0 }, CMD_SAVE, "Save" },
        { { TCODK_F6,       0, 1, 0, 0, 0, 0, 0 }, CMD_LOAD, "Load" },
        { { TCODK_CHAR,   'x', 1, 0, 0, 0, 0, 0 }, CMD_LEAVEDUNGEON, "Exit dungeon" },
};


cmd_t *currentcommands;
int num_commands;
char ret[50];

int cmp_keystruct(TCOD_key_t a, TCOD_key_t b)
{
        if((a.vk == b.vk) &&
           /*(a.pressed == b.pressed) &&*/
           (a.c == b.c) &&
           (a.lalt == b.lalt) &&
           (a.lctrl == b.lctrl) &&
           (a.ralt == b.ralt) &&
           (a.rctrl == b.rctrl) &&
           (a.shift == b.shift))
                return 1;                              /* they're the same */
        else
                return 0;
}

int get_command()
{
        TCOD_key_t key = TCOD_console_wait_for_keypress(false);
        int i;

        if(key.c == 'q')                   // so we can exit easily even if the command system breaks down!
                return CMD_QUIT;

/*        if(key.vk == TCODK_SHIFT) {
                key = TCOD_console_wait_for_keypress(true);
                key.c -= 32;
        }*/

        for(i=0;i<num_commands;i++) {
                if(cmp_keystruct(currentcommands[i].key, key))
                        return currentcommands[i].cmd;
        }

        return 0;
}

char* get_command_text(int i)
{

        if(!strcmp(currentcommands[i].desc, "")) {
                strcpy(ret, "");
                return ret;
        }

        switch(currentcommands[i].key.vk) {
                case TCODK_CHAR:
                        sprintf(ret, "%c - ", currentcommands[i].key.c);
                        break;
                case TCODK_LEFT:
                        sprintf(ret, "Left - ");
                        break;
                case TCODK_RIGHT:
                        sprintf(ret, "Right - ");
                        break;
                case TCODK_UP:
                        sprintf(ret, "Up - ");
                        break;
                case TCODK_DOWN:
                        sprintf(ret, "Down - ");
                        break;
                case TCODK_F5:
                        sprintf(ret, "F5 - ");
                        break;
                case TCODK_F6:
                        sprintf(ret, "F6 - ");
                        break;
                default:
                        sprintf(ret, "... - ");
                        break;
        }

        strcat(ret, currentcommands[i].desc);

        return ret;
}

void switch_commandset(cmd_t *new)
{
        /* ugly hack... */
        if(new == outsidecommands)
                num_commands = (sizeof(outsidecommands)) / sizeof(cmd_t);
        else if(new == dungeoncommands)
                num_commands = (sizeof(dungeoncommands)) / sizeof(cmd_t);

        currentcommands = new;
}

void init_commands()
{
        currentcommands = outsidecommands;
        num_commands = (sizeof(outsidecommands)) / (sizeof(cmd_t));
}
