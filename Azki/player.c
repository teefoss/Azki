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

void P_PlayerInput (void)
{
    // movement
    if (keys[SDL_SCANCODE_W])
        player->dy = -1;
    if (keys[SDL_SCANCODE_S])
        player->dy = 1;
    if (keys[SDL_SCANCODE_A])
        player->dx = -1;
    if (keys[SDL_SCANCODE_D])
        player->dx = 1;
    
    // shoot
    if (keys[SDL_SCANCODE_UP] && !player->delay)
        P_FireBullet(DIR_NORTH);
    if (keys[SDL_SCANCODE_DOWN] && !player->delay)
        P_FireBullet(DIR_SOUTH);
    if (keys[SDL_SCANCODE_LEFT] && !player->delay)
        P_FireBullet(DIR_WEST);
    if (keys[SDL_SCANCODE_RIGHT] && !player->delay)
        P_FireBullet(DIR_EAST);
}



void P_FireBullet (dir_t dir)
{
    int damage;
    int dx, dy;
    
    damage = rand() % 3 + 5;
    dx = dy = 0;
    
    switch (dir)
    {
        case DIR_EAST:
            dx = 1;
            break;
        case DIR_NORTH:
            dy = -1;
            break;
        case DIR_WEST:
            dx = -1;
            break;
        case DIR_SOUTH:
            dy = 1;
            break;
        default:
            break;
    }
    A_SpawnProjectile(TYPE_PROJ_BALL, player, NULL, dx, dy, 3, damage);
    player->delay = 20;
}

void P_UpdatePlayer (obj_t * pl)
{
    static int movetics = 0;
    
    if (movetics)
        movetics--;
    if (pl->delay)
        pl->delay--;
    
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



