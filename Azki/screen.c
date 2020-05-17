//
//  screen.c
//  Azki
//
//  Created by Thomas Foster on 5/9/20.
//  Copyright © 2020 Thomas Foster. All rights reserved.
//

#include <SDL2/SDL.h>
#include "video.h"
#include "azki.h"
#include "player.h"
#include "map.h"

char deathmsg[80];


void S_Level ()
{
    SDL_Event event;
    const int offset = 15;
    char buf[10];
    int y;
    
    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "Level %d",map.num);
    y = (game_res.h - TILE_SIZE) / 2;
    
    while (1)
    {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    Quit(NULL);
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_SPACE) {
                        state = STATE_PLAY;
                        return;
                    }
                    return;
                default:
                    break;
            }
        }
        
        Clear(0, 0, 0);
        TextColor(WHITE);
        PrintCenteredString(buf, -1, y - offset);
        TextColor(RED);
        PrintCenteredString(MapName(map.num), -1, y);
        TextColor(WHITE+BLINK);
        PrintCenteredString("[Press SPACE to begin]", -1, y + offset*2);
        Refresh();
    }
}


void S_GameOver ()
{
    SDL_Event event;
    const int offset = 20;
    int y;
    int wait = 120; // 2 seconds
    
    y = (game_res.h - TILE_SIZE) / 2;
    
    while (1)
    {
        if (wait > 0)
            --wait;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    Quit(NULL);
                    break;
                case SDL_KEYDOWN:
                    if (!wait) {
                        state = STATE_LEVELSCREEN;
                        return;
                    }
                    break;
                default:
                    break;
            }
        }
        
        Clear(0, 0, 0);
        TextColor(RED);
        PrintCenteredString(deathmsg, -1, y - offset);
        
        TextColor(RED+BLINK);
        if (!wait)
            PrintCenteredString("Press any key to continue", -1, y + offset);
        
        Refresh();
    }
}



void S_Controls (const char * title, control_t controls[])
{
    SDL_Event event;
    control_t *cont;
    int y;
    
    strncpy(controls[0].action, title, CONTROL_ACTION_LEN);
    
    while (1)
    {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    Quit(NULL);
                    break;
                case SDL_KEYDOWN:
                        return;
                    break;
                default:
                    break;
            }
        }
        
        Clear(0, 0, 0);
        TextColor(RED);
        
        y = TILE_SIZE;
        cont = &controls[0];
        do
        {
            TextColor(BRIGHTWHITE);
            PrintString(cont->key, TILE_SIZE, y);
            TextColor(RED);
            PrintString(cont->action, 10*TILE_SIZE, y);
            cont++;
            y += TILE_SIZE;
        } while (strcmp(cont->key, "stop"));
                 
        Refresh();
    }

}
