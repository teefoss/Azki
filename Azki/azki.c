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
#include "map.h"

#define MS_PER_FRAME 17

int state;
int tics;

char hudmsg[40];
int hudtics;


void HUDMessage(const char * msg)
{
    hudtics = 90;
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
            TextColor(WHITE);
            PrintString(hudmsg, BottomHUD.x, BottomHUD.y);
            --hudtics;
        }
        P_DrawHealth();
        P_DrawInventory();
        DrawMap(&map);
        List_DrawObjects();
        
        PrintMapName();
        Refresh();
        
        tics++;
        LimitFrameRate(60);

    } while (state == GS_PLAY);
    
    List_RemoveAll();
}
