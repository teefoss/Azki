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
#include "player.h"

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



#pragma mark - TYPE_BULLET

void A_BulletSpawn (int x, int y, int dx, int dy, int damage)
{
    obj_t bl;
    
    memset(&bl, 0, sizeof(bl));
    bl.type = TYPE_BULLET;
    bl.glyph = (glyph_t){ CHAR_DOT1, YELLOW, TRANSP };
    bl.x = x;
    bl.y = y;
    bl.dx = dx;
    bl.dy = dy;
    bl.health = damage;
    bl.update = A_UpdateBullet;
    bl.contact = A_BulletContact;
        
    List_AddObject(&bl);
}

void A_FireBulletDir (obj_t *src, dir_t dir, int damage)
{
    switch (dir) {
        case DIR_EAST:
            A_BulletSpawn(src->x + 1, src->y, 1, 0, damage);
            break;
        case DIR_NORTH:
            A_BulletSpawn(src->x, src->y - 1, 0, -1, damage);
            break;
        case DIR_WEST:
            A_BulletSpawn(src->x - 1, src->y, -1, 0, damage);
            break;
        case DIR_SOUTH:
            A_BulletSpawn(src->x, src->y + 1, 0, 1, damage);
            break;
            
        default:
            break;
    }
}


void A_FireBulletTarget (obj_t *from, obj_t *to, int damage)
{
    float dist;
    float dx, dy;
    float xstep, ystep;
    
    dx = to->x - from->x;
    dy = to->y - from->y;
    dist = sqrt(dx*dx + dy*dy);
    xstep = dx / dist;
    ystep = dy / dist;
    
    A_BulletSpawn(from->x, from->y, xstep, ystep, damage);
}

void A_UpdateBullet (obj_t *b)
{
    obj_t * hit;
    
    if (RunTimer(b)) return;
    
    if ( TryMove(b, b->x + b->dx, b->y + b->dy) )
    {
        b->tics = 5;
    }
    else // handle foreground collision
    {
        hit = &currentmap.foreground[b->y + b->dy][b->x + b->dx];
        switch (hit->type) {
            case TYPE_TREE:
                hit->glyph.fg_color = BROWN;
                break;
            default:
                break;
        }
        b->state = objst_remove;
    }
}

void A_BulletContact (obj_t *b, obj_t *hit)
{
    switch (hit->type) {
        case TYPE_SPIDER:
            hit->health -= b->health ;
            break;
            
        default:
            break;
    }
    b->state = objst_remove;
}


void A_NessieUpdate (obj_t *n)
{
    const int len_above = 120;

    if (n->tics)
        n->tics--;

    // timer has run down, flip state
    if (n->tics == 0)
    {
        if (n->state == objst_active)
        {
            n->state = objst_inactive;
            n->tics = random() % 90 + 240;
        }
        else if (n->state == objst_inactive)
        {
            n->state = objst_active;
            n->tics = len_above;
        }
    }
    
    // update based on state
    if (n->state == objst_active)
    {
        n->glyph.character = objdefs[n->type].glyph.character;
        if (n->tics == len_above / 2)
            A_FireBulletTarget(n, player, 5);
    }
    else if (n->state == objst_inactive)
    {
        n->glyph.character = CHAR_NUL;
    }
}
