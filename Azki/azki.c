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



void PlayLoop (void)
{
    SDL_Event event;
    int x, y;
    obj_t *obj;
    int frame_start, frame_end, dt;
    
    // initialize object list
    obj = &currentmap.foreground[0][0];
    for (y=0 ; y<MAP_H ; y++)
    {
        for (x=0 ; x<MAP_W ; x++)
        {
            if (obj->flags & OF_ENTITY)
            {
                if (obj->type == TYPE_PLAYER)
                    player = List_AddObject(obj);
                else
                    List_AddObject(obj);
            }
            obj++;
        }
    }
    
    
    tics = 0;
    do
    {
        frame_start = SDL_GetTicks();
        
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

        if (keys[SDL_SCANCODE_W])
            player->dy = -1;
        if (keys[SDL_SCANCODE_S])
            player->dy = 1;
        if (keys[SDL_SCANCODE_A])
            player->dx = -1;
        if (keys[SDL_SCANCODE_D])
            player->dx = 1;
        
        if (keys[SDL_SCANCODE_UP] && !player->cooldown)
            A_SpawnBullet(player, DIR_NORTH, 20);
        if (keys[SDL_SCANCODE_DOWN] && !player->cooldown)
            A_SpawnBullet(player, DIR_SOUTH, 20);
        if (keys[SDL_SCANCODE_LEFT] && !player->cooldown)
            A_SpawnBullet(player, DIR_WEST, 20);
        if (keys[SDL_SCANCODE_RIGHT] && !player->cooldown)
            A_SpawnBullet(player, DIR_EAST, 20);

            
        // UPDATE
        if (!objlist)
            Error("PlayLoop: objlist is empty!");
        
        obj = objlist;
        do {
            if (obj->update)
                obj->update(obj);
            obj = obj->next;
        } while (obj);

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
        frame_end = SDL_GetTicks();
        dt = frame_end - frame_start;
        if (keys[SDL_SCANCODE_I])
            printf("frame took %d ms, waiting %d ms...\n", dt, MS_PER_FRAME - dt);
        if (dt < MS_PER_FRAME)
            SDL_Delay(MS_PER_FRAME - dt);

    } while (state == GS_PLAY);
    
    List_RemoveAll();
}
