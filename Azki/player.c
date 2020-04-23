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
    if (pl->cooldown)
        pl->cooldown--;
    
    // move player
    if ( (pl->dx || pl->dy) && !movetics )
    {
        if ( TryMove(pl, pl->x + pl->dx, pl->y + pl->dy) )
            movetics = 10;
    }
    if (!keys[SDL_SCANCODE_W] &&
        !keys[SDL_SCANCODE_A] &&
        !keys[SDL_SCANCODE_S] &&
        !keys[SDL_SCANCODE_D])
    {
        movetics = 0;
    }
    pl->dx = 0;
    pl->dy = 0;
}



