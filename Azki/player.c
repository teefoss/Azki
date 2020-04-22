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
    static int movetics = 0;
    
    if (movetics)
        movetics--;
    
    // move player
    if ( (pl->dx || pl->dy) && !movetics )
    {
        if ( TryMove(pl, pl->x + pl->dx, pl->y + pl->dy) )
            movetics = 15;
    }
    pl->dx = 0;
    pl->dy = 0;
}



