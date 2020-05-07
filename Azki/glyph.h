//
//  glyph.h
//  Azki
//
//  Created by Thomas Foster on 4/1/20.
//  Copyright © 2020 Thomas Foster. All rights reserved.
//

#ifndef glyph_h
#define glyph_h

#include <stdio.h>
#include "azki.h"

#define GLYPH_SIZE 8

enum
{
    CHAR_NUL,
    CHAR_FACE_CLEAR,
    CHAR_FACE_SOLID,
    CHAR_HEART,
    CHAR_DIAMOND,
    CHAR_CLUB,
    CHAR_SPADE,
    CHAR_DOT1,
    CHAR_DOT2
};

typedef struct
{
    uint8_t character;  // ASCII char code
    uint8_t fg_color;   // foreground (char) color
    uint8_t bg_color;   // background color
} glyph_t;

void DrawGlyph (glyph_t *glyph, pixel x, pixel y, int shadow_color);
void DrawGlyphAtTile (glyph_t *g, tile x, tile y, int shadow);
void DrawGlyphAtMapTile (glyph_t *glyph, tile x, tile y, int shadow);

#endif /* glyph_h */
