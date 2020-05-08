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
    
    if (sp->hp <= 0) {
        ChangeObject(sp, TYPE_CORPSE, objst_inactive);
        return;
    }
    
    if (RunTimer(sp)) return;
    
    // move spider in a random direction
    moved = false;
    dir = random() % 4 + 1;
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


void A_SpiderContact (obj_t *sp, obj_t *hit)
{
    switch (hit->type) {
        case TYPE_PLAYER:
            DamageObj(hit, 1);
            break;
            
        default:
            break;
    }
}



#pragma mark - TYPE_BULLET

void
A_SpawnProjectile
( objtype_t  type,
  obj_t      *src,
  obj_t      *dst,
  int        dx,
  int        dy,
  int        delay,
  int        damage )
{
    obj_t proj;
    
    proj = NewObjectFromDef(type, src->x, src->y);
    proj.src = src;
    proj.dst = dst;
    proj.dx = dx;
    proj.dy = dy;
    proj.updatedelay = delay;
    proj.hp = damage;

    List_AddObject(&proj);
}




void A_UpdateProjectile (obj_t *proj)
{
    obj_t * hit;
    int checkx, checky;
    
    if (RunTimer(proj)) return;
    
    if (proj->dst) // projectile has a target, home
    {
        proj->dx = sign(proj->dst->x - proj->x);
        proj->dy = sign(proj->dst->y - proj->y);
    }
    
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
    proj->tics = proj->updatedelay;
}

void A_ProjectileContact (obj_t *proj, obj_t *hit)
{
    //printf("proj src: %s, hit: %s", ObjName(proj->src), ObjName(hit));
    if (proj->src == hit)
        return;

    switch (hit->type)
    {
        case TYPE_PLAYER:
            printf("player hit\n");
            break;
        case TYPE_SPIDER:
            hit->hp -= proj->hp ;
            break;
            
        default:
            break;
    }
    
    hit->hp -= proj->hp;
    printf("player hp: %d\n", player->hp);
    proj->state = objst_remove;
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
        damage = 5 * ((random() % 3) + 1);
        if (n->tics == NESSIE_TIME / 2)
            A_SpawnProjectile(TYPE_PROJ_RING, n, player, 0, 0, 10, damage);
    }
    else if (n->state == objst_inactive)
    {
        // hide char (dive)
        n->glyph.character = CHAR_NUL;
    }
}




void A_OgreUpdate (obj_t *ogre)
{
    tile dx, dy;
    int tries;
    
    if (ogre->hp <= 0)
    {
        ChangeObject(ogre, TYPE_CORPSE, objst_inactive);
        return;
    }
    
    if (RunTimer(ogre)) return;
    
    dx = sign(player->x - ogre->x);
    dy = sign(player->y - ogre->y);
    if ( !TryMove(ogre, ogre->x + dx, ogre->y + dy) )
    {
        tries = 20;
        while (tries--) {
            dx = (random() % 3) - 1; // try a random direction -1, 0, or 1
            dy = (random() % 3) - 1;
            if ( TryMove(ogre, ogre->x + dx, ogre->y + dy) )
                break;
        }
    }
    ogre->tics = (random() % 20) + 50;
    if (ogre->hp <= 0)
        ogre->state = objst_remove;
}




