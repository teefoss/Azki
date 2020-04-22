//
//  action.c
//  Azki
//
//  Created by Thomas Foster on 4/20/20.
//  Copyright © 2020 Thomas Foster. All rights reserved.
//

#include <SDL2/SDL.h>
#include "action.h"
#include "azki.h"
#include "video.h"

void A_UpdateWater (obj_t *water)
{
    if (water->tics)
    {
        water->tics--;
        return;
    }
    water->glyph.character = CHAR_NUL;
    
    // draw a wave once and a while
    if (random() % 10000 < 10) // frequency of occurenc
    {
        water->glyph.character = '~';
        water->tics = 50; // length wave stays
    }
}


void A_UpdateSpider (obj_t *sp)
{
    int dir;
    bool moved;
    
    if (sp->tics)
    {
        sp->tics--;
        return;
    }
    
    // move spider in a random direction
    dir = random() % 4;
    switch (dir)
    {
        case DIR_EAST:
            moved = TryMove(sp, sp->x + 1, sp->y);
            break;
        case DIR_NORTH:
            moved = TryMove(sp, sp->x, sp->y - 1);
            break;
        case DIR_WEST:
            moved = TryMove(sp, sp->x - 1, sp->y);
            break;
        case DIR_SOUTH:
            moved = TryMove(sp, sp->x, sp->y + 1);
            break;
            
        default:
            break;
    }
    
    // reset timer if no collision
    if (moved)
        sp->tics = (random() % 30) + 30;
}
