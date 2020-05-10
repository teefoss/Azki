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
                    Quit();
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
    
    y = (game_res.h - TILE_SIZE) / 2;
    
    while (1)
    {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    Quit();
                    break;
                case SDL_KEYDOWN:
                    state = STATE_LEVELSCREEN;
                    return;
                default:
                    break;
            }
        }
        
        Clear(0, 0, 0);
        TextColor(RED);
        PrintCenteredString(deathmsg, -1, y - offset);
        TextColor(RED+BLINK);
        PrintCenteredString("Press any key to continue", -1, y + offset);
        Refresh();
    }
}
