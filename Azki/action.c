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
    if (RunTimer(water)) return;
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
    
    if (RunTimer(sp)) return;
    
    // move spider in a random direction
    moved = false;
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
    }
    
    // reset timer if no collision
    if (moved)
        sp->tics = (random() % 30) + 30;
}



void A_UpdateBullet (obj_t *b)
{
    if (RunTimer(b)) return;
    
    if ( TryMove(b, b->x + b->dx, b->y + b->dy) )
    {
        b->tics = 5;
    }
    else
    {
        b->state = objst_remove;
    }
}


void A_SpawnBullet (obj_t *src, dir_t dir, int cooldown)
{
    obj_t bl;
    
    switch (dir)
    {
        case DIR_EAST:
            bl = NewObject(TYPE_BULLET, src->x + 1, src->y);
            bl.dx = 1;
            break;
        case DIR_NORTH:
            bl = NewObject(TYPE_BULLET, src->x, src->y - 1);
            bl.dy = -1;
            break;
        case DIR_WEST:
            bl = NewObject(TYPE_BULLET, src->x - 1, src->y);
            bl.dx = -1;
            break;
        case DIR_SOUTH:
            bl = NewObject(TYPE_BULLET, src->x, src->y + 1);
            bl.dy = 1;
            break;
        default:
            break;
    }
    
    List_AddObject(&bl);
    src->cooldown = cooldown;
}
