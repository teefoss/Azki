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
#include "map.h"

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

void
A_SpawnProjectile
( objtype_t type,
  obj_t     *src,
  tile      x,
  tile      y,
  float     dx,
  float     dy,
  int       delay,
  int       damage )
{
    obj_t proj;
    
    proj = NewObjectFromDef(type, x, y);
    proj.dx = dx;
    proj.dy = dy;
    proj.delay = delay;

    List_AddObject(&proj);
}





//
// A_ShootProjectile
// Fire a projectile from src to dst. vmult is a velocity multiplier to
// adjust the speed
//
static void
A_ShootProjectile
( objtype_t type,
  obj_t     *src,
  obj_t     *dst,
  int       damage,
  int       delay,
  float     vmult )
{
    float dist;
    float dx, dy;
    float xstep, ystep;
    
    dx = dst->x - src->x;
    dy = dst->y - src->y;
    dist = sqrt(dx*dx + dy*dy);
    xstep = dx / dist * vmult;
    ystep = dy / dist * vmult;
    
    A_SpawnProjectile(type, src, src->x, src->y, xstep, ystep, delay, damage);
}



void A_UpdateProjectile (obj_t *proj)
{
    obj_t * hit;
    int checkx, checky;
    
    if (RunTimer(proj)) return;
    
    if ( !TryMove(proj, proj->x + proj->dx, proj->y + proj->dy) )
    {
        // handle foreground collision
        checkx = proj->x + proj->dx;
        checky = proj->y + proj->dy;
        hit = &map.foreground[checky][checkx];
        switch (hit->type) {
            case TYPE_TREE:
                hit->glyph.fg_color = BROWN;
                break;
            default:
                break;
        }
        // always remove when a projectile hits a solid layer obj
        proj->state = objst_remove;
    }
    proj->tics = proj->delay;
}

void A_ProjectileContact (obj_t *proj, obj_t *hit)
{
    if (proj->src == hit)
        return;
    
    switch (hit->type) {
        case TYPE_SPIDER:
            hit->hp -= proj->hp ;
            break;
            
        default:
            break;
    }
    //b->state = objst_remove;
}




# define NESSIE_TIME    120 // how long nessie stays above water

void A_NessieUpdate (obj_t *n)
{
    int damage;

    // flip state
    if ( !RunTimer(n) )
    {
        if (n->state == objst_active)
        {
            // dive
            n->state = objst_inactive;
            n->tics = random() % 90 + 240;
        }
        else if (n->state == objst_inactive)
        {
            // surface
            n->state = objst_active;
            n->tics = NESSIE_TIME;
        }
    }
    
    // update based on state
    if (n->state == objst_active)
    {
        // show char and shoot at halfway point
        n->glyph.character = objdefs[n->type].glyph.character;
        damage = 5 * (random() % 3) + 1;
        if (n->tics == NESSIE_TIME / 2)
            A_ShootProjectile(TYPE_PROJ_RING, n, player, damage, 4, 1.0f);
    }
    else if (n->state == objst_inactive)
    {
        // hide char (dive)
        n->glyph.character = CHAR_NUL;
    }
}
