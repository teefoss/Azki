//
//  azki.h
//  Azki
//
//  Created by Thomas Foster on 3/30/20.
//  Copyright © 2020 Thomas Foster. All rights reserved.
//

#ifndef azki_h
#define azki_h

#include "glyph.h"
#include "map.h"

#define CTRL (keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_RCTRL])

enum
{
    GS_PLAY,
    GS_EDITOR
};

extern int state;
extern map_t currentmap;
extern map_t * world;
extern const uint8_t * keys;

extern int tics;

void Error (const char * message);
void Quit (void);
void PlayLoop (void);

#endif /* azki_h */
