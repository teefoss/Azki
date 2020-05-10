//
//  azki.h
//  Azki
//
//  Created by Thomas Foster on 3/30/20.
//  Copyright © 2020 Thomas Foster. All rights reserved.
//

#ifndef azki_h
#define azki_h

#define CTRL        (keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_RCTRL])
#define sign(a)     ((a) > 0 ? 1 : (a) < 0 ? -1 : 0)
#define max(a, b)   ((a) > (b) ? (a) : (b))

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

extern char deathmsg[];
extern int tics;

void Error (const char * message);
void Quit (void);
void PlayLoop (void);
void HUDMessage(const char * msg);


// screens.c
void S_Level (void);
void S_GameOver (void);

#endif /* azki_h */
