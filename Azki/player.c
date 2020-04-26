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
#include "action.h"

obj_t * player;

void P_FireBullet (dir_t dir)
{
    int damage;
    
    damage = rand() % 3 + 5;
    
    switch (dir)
    {
        case DIR_EAST:
            A_SpawnProjectile(player->x + 1, player->y, 1, 0, damage);
            break;
        case DIR_NORTH:
            A_SpawnProjectile(player->x, player->y - 1, 0, -1, damage);
            break;
        case DIR_WEST:
            A_SpawnProjectile(player->x - 1, player->y, -1, 0, damage);
            break;
        case DIR_SOUTH:
            A_SpawnProjectile(player->x, player->y + 1, 0, 1, damage);
            break;
            
        default:
            break;
    }
    player->cooldown = 20;
}

void P_UpdatePlayer (obj_t * pl)
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



void P_PlayerContact (obj_t *pl, obj_t *hit)
{

}



