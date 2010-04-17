#ifndef CMD_H
#define CMD_H

/*
 * Handling commands (keyboard input)
 *
 */


typedef struct {
        TCOD_key_t key;
        int cmd;
        char desc[30];
} cmd_t;

#define CMD_LEFT 1
#define CMD_RIGHT 2
#define CMD_UP 3
#define CMD_DOWN 4
#define CMD_QUIT 5
#define CMD_LONGLEFT 6
#define CMD_LONGRIGHT 7
#define CMD_LONGUP 8
#define CMD_LONGDOWN 9
#define CMD_TOGGLEFOV 10
#define CMD_SAVE 11
#define CMD_LOAD 12
#define CMD_ENTERDUNGEON 13
#define CMD_LEAVEDUNGEON 14
#define CMD_PICKUP 15
#define CMD_DROP 16
#define CMD_WIELD 17
#define CMD_IDENTIFYALL 18
#define CMD_SKILLSCREEN 19
#define CMD_NW 20
#define CMD_NE 21
#define CMD_SW 22
#define CMD_SE 23

//extern cmd_t (*currentcommands)[];
extern cmd_t *currentcommands;
extern cmd_t outsidecommands[];
extern cmd_t dungeoncommands[];
extern int   num_commands;

void init_commands();
void switch_commandset(cmd_t *new);
int get_command();
char* get_command_text(int i);

#endif
