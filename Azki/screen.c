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
        SDL_Delay(15);
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
        SDL_Delay(15);
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
        SDL_Delay(15);
    }
}



//
// S_CharacterView
// Display a helpful grid of characters and colors in the editor
//
#define CHAR_GRID_SIZE  16
#define COLOR_START_X   (CHAR_GRID_SIZE * TILE_SIZE + TILE_SIZE*3)
#define COLOR_W         (TILE_SIZE * 4)
#define COLOR_END_X     (COLOR_START_X + COLOR_W)
void S_CharacterViewer (void)
{
    SDL_Event   event;
    int         x, y;
    int         c;
    int         i;
    SDL_Rect    r;
    const       SDL_Rect charbox = { 132, 0, TILE_SIZE*2, TILE_SIZE*16};
    char        buf[4];
    uint32_t    mousestate;
    SDL_Point   mousept;
    bool        overcolors;
    bool        overchars;
    int         chr;
    int         color;
    char        *colornames[] =
    {
        "BLACK",
        "BLUE",
        "GREEN",
        "CYAN",
        "RED",
        "MAGENTA",
        "BROWN",
        "WHITE",
        "GRAY",
        "BRIGHT BLUE",
        "BRIGHT GREEN",
        "BRIGHT CYAN",
        "BRIGHT RED",
        "BRIGHT MAGENTA",
        "YELLOW",
        "BRIGHT WHITE"
    };
    
    while (1)
    {
        mousestate = SDL_GetMouseState(&mousept.x, &mousept.y);
        mousept.x /= windowed_scale; // TODO: fix for just current scale
        mousept.y /= windowed_scale;
        overcolors = (mousept.x >= COLOR_START_X
                      && mousept.x < COLOR_END_X
                      && mousept.y < NUM_CGA_COLORS * TILE_SIZE);
        overchars = mousept.x < 128 && mousept.y < 128;
        color = overcolors ? mousept.y / TILE_SIZE : BRIGHTWHITE;

        
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    Quit(NULL);
                    break;
                case SDL_KEYDOWN:
                    return;
                default:
                    break;
            }
        }
        
        Clear(0, 0, 0);
        TextColor(RED);
                
//      DRAW CHARACTER GRID
        
        r.x = (mousept.x / TILE_SIZE) * TILE_SIZE;
        r.y = (mousept.y / TILE_SIZE) * TILE_SIZE;
        r.w = TILE_SIZE;
        r.h = TILE_SIZE;
        c = 0;
        for ( y=0 ; y<CHAR_GRID_SIZE ; y++ )
        {
            for ( x=0 ; x<CHAR_GRID_SIZE ; x++)
            {
                TextColor(color);
                PrintChar(c++, x * TILE_SIZE, y * TILE_SIZE);
                if (overchars)
                {
                    SetPaletteColor(RED);
                    SDL_RenderDrawRect(renderer, &r);
                }
            }
        }
        
        // get char num if over grid
        chr = overchars ? r.y/TILE_SIZE * 16 + r.x/TILE_SIZE : 0;
        
//      PRINT SELECTED CHAR NUMBER
        
        if (overchars)
        {
            TextColor(RED);
            snprintf(buf, 4, "%d", chr);
            PrintString(buf, 0, 17*TILE_SIZE);
        }
        
//      DRAW CURRENT CHAR
        
        SetPaletteColor(BLACK);
        SDL_RenderFillRect(renderer, &charbox);
        if (overchars)
        {
            for (i=0 ; i<NUM_CGA_COLORS ; i++)
            {
                TextColor(i);
                PrintChar(chr, 128 + TILE_SIZE, i* TILE_SIZE);
            }
        }
                
//      DRAW COLOR BOXES AND NAMES
        
        r.w = COLOR_W;
        r.h = TILE_SIZE;
        for (i=0 ; i<NUM_CGA_COLORS ; i++)
        {
            r.x = COLOR_START_X;
            r.y = i * TILE_SIZE;
            SetPaletteColor(i);
            SDL_RenderFillRect(renderer, &r);
            if (!overcolors)
            {
                TextColor(BRIGHTWHITE);
                PrintString(colornames[i], r.x + r.w + TILE_SIZE, r.y);
            }
        }
        
//      DRAW COLOR SELECTION BOX AND COLORED NAME
        
        if (overcolors)
        {
            r.y = (mousept.y / TILE_SIZE) * TILE_SIZE;
            SetPaletteColor(TRANSP);
            SDL_RenderDrawRect(renderer, &r);
            TextColor(r.y / TILE_SIZE);
            PrintString(colornames[r.y / TILE_SIZE], r.x + r.w + TILE_SIZE, r.y);
        }

        Refresh();
        SDL_Delay(15);
    }
}
