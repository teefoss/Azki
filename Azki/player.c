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
#include "map.h"

obj_t * player;
playeritems_t items;
int maxhealth = 5;


void InitPlayer (void)
{
    memset(&items, 0, sizeof(playeritems_t));
}

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


void P_OpenDoor (obj_t *door)
{
    switch (door->type)
    {
        case TYPE_GOLDDOOR:
            if (items.goldkey)
            {
                *door = NewObjectFromDef(TYPE_NONE, door->x, door->y);
                items.goldkey = false;
            }
            else
            {
                HUDMessage("You need a gold key to open this door!");
            }
            break;
        case TYPE_BLUEDOOR:
            if (items.bluekey)
            {
                *door = NewObjectFromDef(TYPE_NONE, door->x, door->y);
                items.bluekey = false;
            }
            else
            {
                HUDMessage("You need a gold blue to open this door!");
            }
            break;
        case TYPE_GREENDOOR:
            if (items.greenkey)
            {
                *door = NewObjectFromDef(TYPE_NONE, door->x, door->y);
                items.greenkey = false;
            }
            else
            {
                HUDMessage("You need a green key to open this door!");
            }
            break;
        default:
            break;
    }
}

void P_UpdatePlayer (obj_t * pl)
{
    int newx, newy;
    static int movetics = 0;
    obj_t *contact;
    
    if (movetics)
        movetics--;
    if (pl->delay)
        pl->delay--;
    
    // move player
    newx = pl->x + pl->dx;
    newy = pl->y + pl->dy;
    if ( (pl->dx || pl->dy) && !movetics )
    {
        contact = &map.foreground[newy][newx];
        if ( contact->type == TYPE_GOLDDOOR
            || contact->type == TYPE_BLUEDOOR
            || contact->type == TYPE_GREENDOOR )
        {
            P_OpenDoor(contact);
        }
        
        if ( !TryMove(pl, newx, newy) ) {
            
        }
        else
        {
            movetics = 10;
        }
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



void P_CollectItem (obj_t *item, obj_t *entity)
{
    if (entity->type != TYPE_PLAYER)
        return;
    
    printf("item: %s, entity: %s\n", ObjName(item), ObjName(entity));
    
    printf("item type:    %d\n", item->type);
    printf("TYPE_GOLDKEY: %d\n", TYPE_GOLDKEY);
    switch (item->type)
    {
        case TYPE_GOLDKEY:
            items.goldkey = true;
            break;
        case TYPE_BLUEKEY:
            items.bluekey = true;
            break;
        case TYPE_GREENKEY:
            items.greenkey = true;
            break;
        default:
            break;
    }
    
    item->state = objst_remove;
}




void P_DrawInventory (void)
{
    pixel x, y;
    
    y = BottomHUD.y;
    x = maprect.x + maprect.w;
    
    x -= TILE_SIZE;
    if (items.goldkey)
        DrawGlyph(&objdefs[TYPE_GOLDKEY].glyph, x, y, PITCHBLACK);
    x -= TILE_SIZE;
    if (items.bluekey)
        DrawGlyph(&objdefs[TYPE_BLUEKEY].glyph, x, y, PITCHBLACK);
    x -= TILE_SIZE;
    if (items.greenkey)
        DrawGlyph(&objdefs[TYPE_GREENKEY].glyph, x, y, PITCHBLACK);
    
    x -= TILE_SIZE * 5;
    TextColor(BRIGHTWHITE);
    PrintString("Keys", x, y);
}



void P_DrawHealth (void)
{
    int i;
    int hp;
    pixel x, y;
    glyph_t redheart = { CHAR_HEART, RED, TRANSP };
    glyph_t grayheart = { CHAR_HEART, GRAY, TRANSP };
    
    x = TopHUD.x + maprect.w - TILE_SIZE;
    y = TopHUD.y;
    hp = player->hp;
    for (i=0 ; i<maxhealth ; i++)
    {
        if (hp)
            DrawGlyph(&redheart, x, y, BLACK);
        else
            DrawGlyph(&grayheart, x, y, BLACK);
        if (hp) hp--;
        x -= TILE_SIZE;
    }
}
