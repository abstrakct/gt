#ifndef _WORLDVIEW_H
#define _WORLDVIEW_H

#define WV_OPTIMISTIC 0
#define WV_APATHIC    1
#define WV_DEPRESSED  2

#define WV_SEENOTHING    worldviews[0*3+player->worldview]
#define WV_PICKUPNOTHING worldviews[1*3+player->worldview]
#define WV_NOTHINGLEFT   worldviews[2*3+player->worldview]

#define WORLDVIEWS 3

extern char worldviews[WORLDVIEWS*3][50];

char *get_worldview_string(struct creature *c);

#endif
