//
//  player.c
//  Azki
//
//  Created by Thomas Foster on 4/3/20.
//  Copyright © 2020 Thomas Foster. All rights reserved.
//

#include "player.h"
#include "azki.h"
#include "video.h"
#include "glyph.h"

obj_t * player;

void UpdatePlayer (obj_t * pl)
{
    // move player
    if (pl->dx || pl->dy)
    {
        int newx, newy;
        
        newx = pl->x + pl->dx;
        newy = pl->y + pl->dy;
        if ( !(currentmap.foreground[newy][newx].flags & OF_SOLID) )
        {
            pl->x = newx;
            pl->y = newy;
        }
        pl->dx = 0;
        pl->dy = 0;
    }
}



