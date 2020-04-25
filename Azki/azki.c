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
#include "action.h"

#define MS_PER_FRAME 17

int     state;
map_t   currentmap;
map_t * world;
obj_t   objects;

int tics;


//
// InitializeObjectList
// Look through FG and BG layer for entities,
// add to list, and remove from layer
//
void InitializeObjectList (void)
{
    obj_t *obj;
    int i, x, y;
    
    obj = &currentmap.foreground[0][0];
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
        Error("PlayLoop: oops! no player start?");
}


void DoGameInput (void)
{
    SDL_Event event;
    
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
            Quit();
        
        else if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
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
                    state = GS_EDITOR;
                    break;
                    
                case SDLK_MINUS:
                    SetScale(windowed_scale - 1);
                    break;
                case SDLK_EQUALS:
                    SetScale(windowed_scale + 1);
                    break;
                    
                default:
                    break;
            }
        }
    }

    // player movement
    if (keys[SDL_SCANCODE_W])
        player->dy = -1;
    if (keys[SDL_SCANCODE_S])
        player->dy = 1;
    if (keys[SDL_SCANCODE_A])
        player->dx = -1;
    if (keys[SDL_SCANCODE_D])
        player->dx = 1;
    
    // player shoot
    if (keys[SDL_SCANCODE_UP] && !player->cooldown)
        P_FireBullet(DIR_NORTH);
    if (keys[SDL_SCANCODE_DOWN] && !player->cooldown)
        P_FireBullet(DIR_SOUTH);
    if (keys[SDL_SCANCODE_LEFT] && !player->cooldown)
        P_FireBullet(DIR_WEST);
    if (keys[SDL_SCANCODE_RIGHT] && !player->cooldown)
        P_FireBullet(DIR_EAST);
}



void PlayLoop (void)
{
    obj_t *obj, *check;
    
    InitializeObjectList();
    
    tics = 0;
    do
    {
        StartFrame();
        DoGameInput();
            
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
                        check->x == obj->x &&
                        check->y == obj->y)
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
        
        DrawMap(&currentmap);
        List_DrawObjects();
        
        PrintMapName();
        Refresh();
        
        tics++;
        LimitFrameRate(60);

    } while (state == GS_PLAY);
    
    List_RemoveAll();
}
