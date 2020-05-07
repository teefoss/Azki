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
dir_t player_sword;


void InitPlayer (void)
{
    memset(&items, 0, sizeof(playeritems_t));
}



void P_SwingSword (dir_t dir)
{
    player_sword = dir;
    int swordx = 0, swordy = 0;
    obj_t *listobj;
    obj_t *fg_hit;
    
    if (dir == DIR_NONE)
        return;
    
    fg_hit = NULL;
    switch (dir)
    {
        case DIR_NORTH:
            swordx = player->x;
            swordy = player->y - 1;
            if (swordy >= 0)
                fg_hit = &map.foreground[swordy][swordx];
            break;
        case DIR_SOUTH:
            swordx = player->x;
            swordy = player->y + 1;
            if (swordy <= MAP_H - 1)
                fg_hit = &map.foreground[swordy][swordx];
            break;
        case DIR_EAST:
            swordx = player->x + 1;
            swordy = player->y;
            if (swordx <= MAP_W - 1)
                fg_hit = &map.foreground[swordy][swordx];
            break;
        case DIR_WEST:
            swordx = player->x - 1;
            swordy = player->y;
            if (swordx >= 0)
                fg_hit = &map.foreground[swordy][swordx];
            break;
            
        default:
            break;
    }
    
    if (fg_hit) {
        printf("hit type: %s\n",ObjName(fg_hit));
        fg_hit->hp--;
        if ((fg_hit->flags & OF_BREAKABLE) && fg_hit->hp <= 0)
            *fg_hit = NewObjectFromDef(TYPE_NONE, fg_hit->x, fg_hit->y); // remove it
    }
    
    listobj = objlist;
    do
    {
        if (listobj->x == swordx && listobj->y == swordy)
        {
            listobj->hp--;
        }
        listobj = listobj->next;
    } while (listobj);
}


void P_DrawSword (void)
{
    glyph_t sword = { 0, WHITE, TRANSP };
    
    switch (player_sword)
    {
        case DIR_NONE:
            return;
            
        case DIR_NORTH:
            if (player->y >= 1) {
                sword.character = 179;
                DrawGlyphAtMapTile(&sword, player->x, player->y - 1, PITCHBLACK);
            }
            break;
        case DIR_SOUTH:
            if (player->y <= MAP_H - 2) {
                sword.character = 179;
                DrawGlyphAtMapTile(&sword, player->x, player->y + 1, PITCHBLACK);
            }
            break;
        case DIR_EAST:
            if (player->x <= MAP_W - 2) {
                sword.character = 196;
                DrawGlyphAtMapTile(&sword, player->x+1, player->y, PITCHBLACK);
            }
            break;
        case DIR_WEST:
            if (player->x >= 1) {
                sword.character = 196;
                DrawGlyphAtMapTile(&sword, player->x-1, player->y, PITCHBLACK);
            }
            break;
            
        default:
            break;
    }
}


void P_PlayerInput (void)
{
    // movement
    if (keys[SDL_SCANCODE_W])
        player->dy = -1;
    if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_X])
        player->dy = 1;
    if (keys[SDL_SCANCODE_A])
        player->dx = -1;
    if (keys[SDL_SCANCODE_D])
        player->dx = 1;
    
    // diagonals
    if (keys[SDL_SCANCODE_Q]) {
        player->dx = -1;
        player->dy = -1;
    }
    if (keys[SDL_SCANCODE_E]) {
        player->dx = 1;
        player->dy = -1;
    }
    if (keys[SDL_SCANCODE_Z]) {
        player->dx = -1;
        player->dy = 1;
    }
    if (keys[SDL_SCANCODE_C]) {
        player->dx = 1;
        player->dy = 1;
    }
        
    
    // shoot
#if 0
    if (keys[SDL_SCANCODE_UP] && !player_sword)
        P_FireBullet(DIR_NORTH);
    if (keys[SDL_SCANCODE_DOWN] && !player_sword)
        P_FireBullet(DIR_SOUTH);
    if (keys[SDL_SCANCODE_LEFT] && !player_sword)
        P_FireBullet(DIR_WEST);
    if (keys[SDL_SCANCODE_RIGHT] && !player_sword)
        P_FireBullet(DIR_EAST);
#endif
    if (player_sword == DIR_NONE) {
        if (keys[SDL_SCANCODE_UP])
            P_SwingSword(DIR_NORTH);
        if (keys[SDL_SCANCODE_DOWN])
            P_SwingSword(DIR_SOUTH);
        if (keys[SDL_SCANCODE_LEFT])
            P_SwingSword(DIR_WEST);
        if (keys[SDL_SCANCODE_RIGHT])
            P_SwingSword(DIR_EAST);
    }

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


bool P_HasKey (obj_t *door)
{
    switch (door->type)
    {
        case TYPE_GOLDDOOR:
            if (items.goldkey)
                return true;
            break;
        case TYPE_BLUEDOOR:
            if (items.bluekey)
                return true;
            break;
        case TYPE_GREENDOOR:
            if (items.greenkey)
                return true;
            break;
        default:
            printf("Weird door type!?");
            break;
    }
    return false;
}



void P_TryOpenDoor (obj_t *door)
{
    if (P_HasKey(door))
        RemoveObj(door);
    else
        HUDMessage(door->info->hud);
}



void P_UpdatePlayer (obj_t * pl)
{
    int newx, newy;
    static int movetics = 0;
    obj_t *contact;
    const int movedelay = 10;
    
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
            P_TryOpenDoor(contact);
        }
        
        if ( !TryMove(pl, newx, newy) ) {
            if (contact->type == TYPE_WATER && items.boat) {
                player->x = newx;
                player->y = newy;
                movetics = movedelay * 2;
            }
        }
        else
        {
            movetics = movedelay;
        }
    }
        
    if (!keys[SDL_SCANCODE_W]
        && !keys[SDL_SCANCODE_A]
        && !keys[SDL_SCANCODE_S]
        && !keys[SDL_SCANCODE_D]
        && !keys[SDL_SCANCODE_E]
        && !keys[SDL_SCANCODE_Z]
        && !keys[SDL_SCANCODE_C]
        && !keys[SDL_SCANCODE_X]
        && !keys[SDL_SCANCODE_Q])
    {
        movetics = 0;
    }
    pl->dx = 0;
    pl->dy = 0;
}



void P_PlayerContact (obj_t *pl, obj_t *hit)
{
    pl->hp -= hit->hp;
    printf("player contact: hp %d\n", pl->hp);
}



void P_CollectItem (obj_t *item, obj_t *entity)
{
    if (entity->type != TYPE_PLAYER)
        return;
    
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
            
        case TYPE_HEART:
            if (++player->hp > maxhealth)
                player->hp = maxhealth;
            
        case TYPE_BOAT:
            items.boat = true;
        default:
            break;
    }
    
    HUDMessage(objdefs[item->type].hud);
    item->state = objst_remove;
}


void P_DrawPlayer (void)
{
    SDL_Rect raft;
    
    if (items.boat && map.foreground[player->y][player->x].type == TYPE_WATER)
    {
        SetPaletteColor(BROWN);
        raft.x = player->x * TILE_SIZE + maprect.x - 1;
        raft.y = player->y * TILE_SIZE + maprect.y - 1;
        raft.w = TILE_SIZE + 3;
        raft.h = TILE_SIZE + 3;
        SDL_RenderFillRect(renderer, &raft);
    }
    
    DrawGlyphAtMapTile(&player->glyph, player->x, player->y, PITCHBLACK);
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
