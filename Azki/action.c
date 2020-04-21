//
//  action.c
//  Azki
//
//  Created by Thomas Foster on 4/20/20.
//  Copyright © 2020 Thomas Foster. All rights reserved.
//

#include <SDL2/SDL.h>
#include "action.h"
#include "azki.h"
#include "video.h"

void A_UpdateWater (obj_t *water)
{
    if (water->tics)
    {
        water->tics--;
        return;
    }
    water->glyph.character = CHAR_NUL;
    
    // draw a wave once and a while
    if (random() % 10000 < 10) // frequency of occurenc
    {
        water->glyph.character = '~';
        water->tics = 50; // length wave stays
    }
}
