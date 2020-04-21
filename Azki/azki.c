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
                        
                    case SDLK_UP:
                        player->dy = -1;
                        break;
                    case SDLK_DOWN:
                        player->dy = 1;
                        break;
                    case SDLK_LEFT:
                        player->dx = -1;
                        break;
                    case SDLK_RIGHT:
                        player->dx = 1;
                        break;
                        
                    default:
                        break;
                }
            }
        }
        
        // UPDATE
        if (!objlist)
            Error("PlayLoop: objlist is empty!");
        
        obj = objlist;
        do {
            if (obj->update)
                obj->update(obj);
            obj = obj->next;
        } while (obj);
        
        
        Clear(0, 0, 0);
        TextColor(RED);
        
        DrawMap(&currentmap);
        List_DrawObjects();
        
        PrintMapName();
        Refresh();
        
        SDL_Delay(10);
        tics++;
    } while (state == GS_PLAY);
    
    List_RemoveAll();
}
