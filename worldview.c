#include <string.h>
#include <libtcod.h>

#include "gt.h"
#include "objects.h"
#include "world.h"
#include "creature.h"
#include "display.h"
#include "you.h"
#include "worldview.h"

// OPTIMISTIC; APATHIC; DEPRESSED

char worldviews[WORLDVIEWS*3][50] = {
        "You see nothing here that could interest you.", "You see the world as it is.", "You see the whole snafu.",
        "pick up an imaginary girl.", "try to grasp the void.", "fail at picking up some air.",
        "have rid yourself of all earthly posessions!", "drop out.", "are ruined!",
};

char *get_worldview_string(creature_t *c)
{
        static char ret[15];

        switch(c->worldview) {
                case WV_OPTIMISTIC:
                        strcpy(ret, "Optimistic :)");
                        break;
                case WV_APATHIC:
                        strcpy(ret, "Apathic :|");
                        break;
                case WV_DEPRESSED:
                        strcpy(ret, "Depressed :(");
                        break;
                default:
                        strcpy(ret, "Unknown!");
                        break;
        }

        return ret;
}
                       
void init_worldview()
{
        return;
}
