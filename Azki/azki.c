//
//  azki.c
//  Azki
//
//  Created by Thomas Foster on 3/30/20.
//  Copyright © 2020 Thomas Foster. All rights reserved.
//

#include <SDL2/SDL.h>
#include "azki.h"
#include "video.h"
#include "player.h"
#include "map.h"

#define MS_PER_FRAME 17

int state;
int tics;

char hudmsg[40];
int hudtics;


void HUDMessage(const char * msg)
{
    hudtics = 120;
    strncpy(hudmsg, msg, 40);
}



//
// InitializeObjectList
// Look through FG and BG layer for entities,
// add to list, and remove from layer
//
void InitializeObjectList (void)
{
    obj_t *obj;
    int i, x, y;
    
    obj = &map.foreground[0][0];
    for (i=0 ; i<MAP_W*MAP_H ; i++, obj++)
    {
        if (obj->flags & OF_ENTITY)
        {
            x = obj->x;
            y = obj->y;
            
            if (obj->type == TYPE_PLAYER)
                player = List_AddObject(obj);
            else
                List_AddObject(obj);
            
            *obj = NewObjectFromDef(TYPE_NONE, x, y); // remove it
        }
    }
    
    if (player == NULL)
        Error("InitializeObjectList: oops! no player start?");
}


void DrawCompass (void)
{
    SDL_Rect box;
    objtype_t n, e, s, w;
    int nlen, elen, slen, wlen;
    int centerx, centery;
    int nsmax, ewmax;
    
    centerx = (game_res.w - TILE_SIZE) / 2;
    centery = (game_res.h - TILE_SIZE) / 2;

    n = TYPE_NONE;
    e = TYPE_NONE;
    s = TYPE_NONE;
    w = TYPE_NONE;
    nlen = 0;
    elen = 0;
    slen = 0;
    wlen = 0;
    if (player->y >= 1)
    {
        n = ObjectTypeAtXY(player->x, player->y - 1);
        if (n == TYPE_NONE) {
            n = List_ObjectAtXY(player->x, player->y - 1);
        }
        nlen = (n == TYPE_NONE) ? 0 : (int)strlen(objdefs[n].name);
    }
    if (player->x <= MAP_W - 2)
    {
        e = ObjectTypeAtXY(player->x + 1, player->y);
        if (e == TYPE_NONE) {
            e = List_ObjectAtXY(player->x + 1, player->y);
        }
        elen = (e == TYPE_NONE) ? 0 : (int)strlen(objdefs[e].name);
    }
    if (player->y <= MAP_H - 2)
    {
        s = ObjectTypeAtXY(player->x, player->y + 1);
        if (s == TYPE_NONE) {
            s  = List_ObjectAtXY(player->x, player->y + 1);
        }
        slen = (s == TYPE_NONE) ? 0 : (int)strlen(objdefs[s].name);
    }
    if (player->x >= 1)
    {
        w = ObjectTypeAtXY(player->x - 1, player->y);
        if (w == TYPE_NONE) {
            w = List_ObjectAtXY(player->x - 1, player->y);
        }
        wlen = (w == TYPE_NONE) ? 0 : (int)strlen(objdefs[w].name);
    }
    
    if (nlen > slen) {
        nsmax = nlen * 2 + 5;
    } else {
        nsmax = slen * 2 + 5;
    }
    if (wlen > elen) {
        ewmax = wlen * 2 + 10;
    } else {
        ewmax = elen * 2 + 10;
    }
    
    box.w = nsmax > ewmax ? nsmax * TILE_SIZE : ewmax * TILE_SIZE;
    box.h = 11 * TILE_SIZE;
    box.x = (game_res.w - box.w) / 2;
    box.y = (game_res.h - box.h) / 2;
    SetPaletteColor(PITCHBLACK);
    SDL_RenderFillRect(renderer, &box);

    TextColor(BRIGHTWHITE);
    BackgroundColor(PITCHBLACK);
    PrintString("N", centerx, centery - TILE_SIZE);
    PrintString("S", centerx, centery + TILE_SIZE);
    PrintString("E", centerx + TILE_SIZE, centery);
    PrintString("W", centerx - TILE_SIZE, centery);
    TextColor(WHITE);
    PrintChar(24, centerx, centery - TILE_SIZE*2);
    PrintChar(25, centerx, centery + TILE_SIZE*2);
    PrintChar(27, centerx - TILE_SIZE*2, centery);
    PrintChar(26, centerx + TILE_SIZE*2, centery);
    TextColor(BRIGHTWHITE);
    if (n != TYPE_NONE)
    {
        DrawGlyph(&objdefs[n].glyph, centerx, centery - TILE_SIZE*4, TRANSP);
        PrintString(objdefs[n].name, centerx + TILE_SIZE*1.5, centery - TILE_SIZE*4);
    }
    if (s != TYPE_NONE)
    {
        DrawGlyph(&objdefs[s].glyph, centerx, centery + TILE_SIZE*4, TRANSP);
        int len = (int)strlen(objdefs[s].name);
        PrintString(objdefs[s].name, centerx-(len*TILE_SIZE)-TILE_SIZE/2, centery + TILE_SIZE*4);
    }
    if (e != TYPE_NONE)
    {
        DrawGlyph(&objdefs[e].glyph, centerx + TILE_SIZE*4, centery, TRANSP);
        PrintString(objdefs[e].name, centerx + TILE_SIZE*4, centery + TILE_SIZE*1.5);
    }
    if (w != TYPE_NONE)
    {
        DrawGlyph(&objdefs[w].glyph, centerx - TILE_SIZE*4, centery, TRANSP);
        int len = (int)strlen(objdefs[w].name);
        PrintString(objdefs[w].name, centerx - TILE_SIZE*3 - len*TILE_SIZE, centery - TILE_SIZE*1.5);
    }
}



void GameKeyDown (SDL_Keycode key)
{
    switch (key)
    {
        case SDLK_ESCAPE:
            Quit();
            break;
            
        case SDLK_f:
            if (CTRL)
                ToggleFullscreen();
            break;
        case SDLK_BACKSLASH:
            ToggleFullscreen();
            break;
            
        case SDLK_BACKQUOTE:
            state = STATE_EDIT;
            break;
            
        case SDLK_MINUS:
            SetScale(windowed_scale - 1);
            break;
        case SDLK_EQUALS:
            SetScale(windowed_scale + 1);
            break;
            
        case SDLK_i: {
            //printf("num list objects: %d\n", List_Count());
            obj_t *test = &map.foreground[0][0];
            int dist = ObjectDistance(player, test);
            printf("dist to (0,0): %d\n", dist);
            break;
        }
            
        default:
            break;
    }
}


void DoGameInput (void)
{
    SDL_Event event;
    
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                Quit();
                break;
                
            case SDL_KEYDOWN:
                GameKeyDown(event.key.keysym.sym);
                break;
                
            case SDL_KEYUP:
                switch (event.key.keysym.sym)
                {
                    case SDLK_UP:
                    case SDLK_DOWN:
                    case SDLK_LEFT:
                    case SDLK_RIGHT:
                        player_sword = DIR_NONE;
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
            }
        }
    }






void PlayLoop (void)
{
    obj_t *obj, *check;
    
    InitializeObjectList();
    InitPlayer();
    
    tics = 0;
    do
    {
        StartFrame();
        DoGameInput();
        P_PlayerInput();
            
        // UPDATE
                
        // update positions
        obj = objlist;
        do {
            if (obj->update)
                obj->update(obj);
            obj = obj->next;
        } while (obj);
        
#if 1
        // handle any collisions
        obj = objlist;
        do {
            if (obj->state)
            {
                check = obj->next;
                while (check)
                {
                    if (check->state &&
                        check->x == (int)obj->x && // use interger tile coords!
                        check->y == (int)obj->y)
                    {
                        if (obj->contact)
                            obj->contact(obj, check);
                        if (check->contact)
                            check->contact(check, obj);
                        
                        if (!obj->state) // check removed obj
                            break;
                    }
                    check = check->next;
                }
            }
            obj = obj->next;
        } while (obj);
#endif

        // remove removables
        obj = objlist;
        do {
            if ( obj->state == objst_remove )
                obj = List_RemoveObject(obj);
            else
                obj = obj->next;
        } while (obj);

        
        Clear(0, 0, 0);
        TextColor(RED);
        
        if (hudtics)
        {
            TextColor(YELLOW);
            PrintString(hudmsg, BottomHUD.x, BottomHUD.y);
            --hudtics;
        }
        P_DrawHealth();
        P_DrawInventory();
        DrawMap(&map);
        List_DrawObjects();
        P_DrawSword();
        P_DrawPlayer();
        if (keys[SDL_SCANCODE_TAB])
        {
            DrawCompass();
        }
        
        PrintMapName();
        Refresh();
        
        tics++;
        LimitFrameRate(60);

    } while (state == STATE_PLAY);
    
    List_RemoveAll();
}
