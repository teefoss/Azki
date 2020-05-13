//
//  azki.h
//  Azki
//
//  Created by Thomas Foster on 3/30/20.
//  Copyright © 2020 Thomas Foster. All rights reserved.
//

#ifndef azki_h
#define azki_h

#include <stdint.h>

#define DEVELOPMENT
#define TILE_SIZE       8       // tiles are 8 x 8 pixels
#define CTRL            (keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_RCTRL])

typedef int tile;
typedef int pixel;

enum
{
    STATE_LEVELSCREEN,
    STATE_PLAY,
    STATE_GAMEOVER,
    STATE_EDIT
};

extern int state;
extern const uint8_t * keys;

extern int tics;

void Quit (const char * error);
void PlayLoop (void);
void HUDMessage(const char * msg);


// -----------------------------------------------------------------------------
// screen.c

extern char deathmsg[];

void S_Level (void);
void S_GameOver (void);


// -----------------------------------------------------------------------------
// editor.c

typedef enum {
    LAYER_FG,
    LAYER_BG,
    LAYER_BOTH
} layerview_t;

extern layerview_t showlayer;
void EditorLoop (void);

#endif /* azki_h */
