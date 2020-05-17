//
//  action.c
//  Azki
//
//  Created by Thomas Foster on 4/20/20.
//  Copyright © 2020 Thomas Foster. All rights reserved.
//

#include <SDL2/SDL.h>
#include "cmdlib.h"
#include "video.h"
#include "player.h"
#include "map.h"


#pragma mark - Environment

void A_UpdateWater (obj_t *water)
{
    if (--water->tics > 0)
        return;
    
    water->glyph.character = CHAR_NUL;
    
    // draw a wave once and a while
    if (Random() % 10000 < 7) // frequency of occurence
    {
        water->glyph.character = '~';
        water->tics = 50; // length wave stays
    }
}



void A_ExitContact (obj_t *exit, obj_t *hit)
{
    switch (hit->type) {
        case TYPE_PLAYER:
            NextLevel(+1);
            state = STATE_LEVELSCREEN;
            break;
            
        default:
            List_RemoveObject(hit);
            break;
    }
}


void A_Flicker (obj_t *obj)
{
    uint8_t *currentcolor;
    uint8_t *originalcolor;
    
    if (--obj->tics > 0)
        return;
    
    currentcolor = &obj->glyph.fg_color;
    originalcolor = &objdefs[obj->type].glyph.fg_color;
    if (*currentcolor == *originalcolor) {
        *currentcolor = BRIGHTWHITE;
        obj->updatedelay = Random() % 5;
    }
    else {
        *currentcolor = *originalcolor;
        obj->tics = (Random() % 5) + 12;
    }
        
}




#pragma mark - Projectiles

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
    
    if (--proj->tics > 0)
        return;
    
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
            
        default:
            DamageObj(proj->src, hit, proj->hp);
            break;
    }
    
    //printf("player hp: %d\n", player->hp);
    proj->state = objst_remove;
}



#pragma mark - Enemies

// Move around Randomly, or close on player if close
void A_SpiderUpdate (obj_t *sp)
{
    int dir;
    bool moved;
    
    if (sp->hp <= 0) {
        ChangeObject(sp, TYPE_CORPSE, objst_inactive);
        return;
    }
    
    if (--sp->tics > 0)
        return;

    
    moved = false;
    if (ObjectDistance(sp, player.obj) > 5)
    {
        moved = TryMoveRandom4(sp);
    }
    else // spider is close, home in on player
    {
        dir = Random() % 2; // pick a Random direction, x or y
        switch (dir) {
            case 0: { // move in x dir
                int newx = sp->x + sign(player.obj->x - sp->x);
                moved = TryMove(sp, newx, sp->y);
                break;
            }
            case 1: { // move in y dir
                int newy = sp->y + sign(player.obj->y - sp->y);
                moved = TryMove(sp, sp->x, newy);
                break;
            }
        }
    }
    
    // reset timer if no collision
    if (moved)
        sp->tics = (Random() % 30) + 30;
}


void A_SpiderContact (obj_t *sp, obj_t *hit)
{
    switch (hit->type) {
        case TYPE_PLAYER:
            DamageObj(sp, hit, 1);
            break;
            
        default:
            break;
    }
}



# define NESSIE_TIME    120 // how long nessie stays above water

void A_NessieUpdate (obj_t *n)
{
    int damage;

    // flip state, above or below water
    if ( --n->tics > 0 )
    {
        if (n->state == objst_active)
        {
            // dive
            n->state = objst_inactive;
            n->tics = Random() % 90 + 240;
        }
        else if (n->state == objst_inactive)
        {
            // surface
            n->state = objst_active;
            n->tics = NESSIE_TIME;
        }
    }
    
    // update based on state
    if (n->state == objst_active) // above water
    {
        // show char and shoot at halfway point
        n->glyph.character = objdefs[n->type].glyph.character;
        damage = 5 * ((Random() % 3) + 1);
        if (n->tics == NESSIE_TIME / 2)
            A_SpawnProjectile(TYPE_PROJ_RING, n, player.obj, 0, 0, 10, damage);
    }
    else
    if (n->state == objst_inactive) // below water
    {
        n->glyph.character = CHAR_NUL; // hide char (dive)
    }
}




void A_OgreUpdate (obj_t *ogre)
{
    tile dx, dy;
    int tries;
    
    FlashObject(ogre, &ogre->hittimer, BRIGHTWHITE);
    
    if (ogre->hp <= 0)
    {
        ChangeObject(ogre, TYPE_CORPSE, objst_inactive);
        return;
    }
    
    if (--ogre->tics > 0)
        return;
    
    dx = sign(player.obj->x - ogre->x);
    dy = sign(player.obj->y - ogre->y);
    if ( !TryMove(ogre, ogre->x + dx, ogre->y + dy) )
    {
        tries = 20;
        while (tries--) {
            dx = (Random() % 3) - 1; // try a Random direction -1, 0, or 1
            dy = (Random() % 3) - 1;
            if ( TryMove(ogre, ogre->x + dx, ogre->y + dy) )
                break;
        }
    }
    ogre->tics = (Random() % 30) + 70;
    if (ogre->hp <= 0)
        ogre->state = objst_remove;
}



void A_OgreContact (obj_t *ogre, obj_t *hit)
{
    if (hit->type == TYPE_PLAYER)
    {
        DamageObj(ogre, hit, 2);
    }
}

SDL_Point pathdir[7] =
{
    { 1,  1},
    { 1,  0},
    { 0,  1},
    { 1, -1},
    { 0, -1},
    {-1,  0},
    {-1, -1},
};

void A_BlobUpdate (obj_t *blob)
{
    int dx, dy;
    int i, try_x, try_y;
    
    if (blob->hp <= 0) {
        ChangeObject(blob, TYPE_CORPSE, objst_inactive);
        return;
    }
    
    if (ObjectDistance(blob, player.obj) > 7) {
        return; // do nothing until close to the player
    }
    
    if (--blob->tics > 0)
        return;

    dx = sign(player.obj->x - blob->x);
    dy = sign(player.obj->y - blob->y);
    
    // path find?
    for (i=0 ; i<7 ; i++)
    {
        try_x = blob->x + dx * pathdir[i].x;
        try_y = blob->y + dy * pathdir[i].y;
        
        if ( TryMove(blob, try_x, try_y) )
            break;
    }
    blob->tics = 15;
}


void A_EnemyContact (obj_t *enemy, obj_t *hit)
{
    switch (hit->type) {
        case TYPE_PLAYER:
            DamageObj(enemy, hit, enemy->damage);
            break;
            
        default:
            break;
    }
}












