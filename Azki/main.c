//
//  main.c
//  Azki
//
//  Created by Thomas Foster on 3/30/20.
//  Copyright © 2020 Thomas Foster. All rights reserved.
//

/*
 Influences:
    Kye: environmental puzzles
    Blockworld, Bryte: block pushing
    Zelda, DOOM: Combat
 
 IDEAS:
    Weapon: The Arcane Bazooka
    Weapon: Semi-automatic Nerf Gun
 
 TODO:
    Handle player death - restart level
    collectible items
 
 BUGS:
 
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>

#include "azki.h"
#include "video.h"
#include "map.h"
#include "cmdlib.h"

const uint8_t * keys;


void Quit (const char * error)
{
    List_RemoveAll();
    ShutdownVideo();
    SDL_Quit();
    if (error && *error) {
        puts(error);
        puts("\n");
        exit(1);
    }
    exit(0);
}




int main(int argc, char ** argv)
{
    int i;
    int mapnum;
    void EditorLoop (void);
    
//    char buf[120];
//    getcwd(buf, sizeof(buf));
//    printf("current working directory: %s\n", buf);

    myargc = argc;
    myargv = argv;
    
    StartVideo();
    SeedRandom( (unsigned)time(NULL) );
    
    keys = SDL_GetKeyboardState(NULL);
    maprect.w = MAP_W * TILE_SIZE;
    maprect.h = MAP_H * TILE_SIZE;
    UpdateDrawLocations(windowed_scale);
        
    i = CheckParameter("-edit");
    if (i && i+1 <= argc) {
        state = STATE_EDIT;
        sscanf(argv[i+1], "%d", &mapnum);
        
        if (!LoadMap(mapnum, &map)) {
            if ( !NewMap(mapnum, &map) ) {
                Quit("Could not create new map!");
            }
        }
    } else {
        state = STATE_LEVELSCREEN;
        if ( !LoadMap(1, &map) ) {
#ifdef DEVELOPMENT
            NewMap(1, &map); // in dev, create a new map if none found
            state = STATE_EDIT;
#else
            Quit("Could not load starting map!");
#endif
        }
    }
    
    while (1)
    {
        switch (state)
        {
            case STATE_LEVELSCREEN:
            {
                S_Level();
                break;
            }
            case STATE_PLAY:
            {
                PlayLoop();
                break;
            }
            case STATE_GAMEOVER:
            {
                S_GameOver();
            }
            case STATE_EDIT:
            {
                EditorLoop();
                break;
            }
            default: Quit("state = ?????");
        }
    }
}
