//
//  glyph.c
//  Azki
//
//  Created by Thomas Foster on 4/1/20.
//  Copyright © 2020 Thomas Foster. All rights reserved.
//

#include "glyph.h"
#include "video.h"
#include "map.h"


//
//  DrawGlyph
//  Draw glyph and shadow at window x, y
//
void DrawGlyph (glyph_t *glyph, pixel x, pixel y, int shadow_color)
{
    SDL_Rect src;   // rect in the sprite sheet
    SDL_Rect dst;   // destination rect
    
    if (glyph->character == CHAR_NUL && glyph->bg_color == TRANSP)
        return; // don't bother
    
    src.w = src.h = dst.w = dst.h = TILE_SIZE;
    src.x = glyph->character * TILE_SIZE;

    // background shadow
#if 1
    if (shadow_color != TRANSP && glyph->bg_color != TRANSP)
    {
        dst.x = x + 1;
        dst.y = y + 1;
        SetPaletteColor(PITCHBLACK);
        SDL_RenderFillRect(renderer, &dst);
    }
#endif
    
    // background color
    if (glyph->bg_color != TRANSP)
    {
        SetPaletteColor(glyph->bg_color);
        FillRect(x, y, TILE_SIZE, TILE_SIZE);
    }
    
    // glyph shadow (only on TRANPS bkgr)
    if (shadow_color != TRANSP && glyph->bg_color == TRANSP)
    {
        src.y = shadow_color * TILE_SIZE;
        dst.x = x + 1;
        dst.y = y + 1;
        SDL_RenderCopy(renderer, font_table, &src, &dst);
    }
        
    // glyph
    if (glyph->fg_color != TRANSP)
    {
        if (glyph->fg_color & BLINK) {
            int color = glyph->fg_color;
            color ^= BLINK;
            if (SDL_GetTicks() % 600 < 300)
                color ^= 8;
            src.y = (color % NUMCOLORS) * TILE_SIZE;
        } else {
            src.y = glyph->fg_color * TILE_SIZE;
        }
        dst.x = x;
        dst.y = y;
        SDL_RenderCopy(renderer, font_table, &src, &dst);
    }
}


void DrawGlyphAtTile (glyph_t *g, tile x, tile y, int shadow)
{
    DrawGlyph(g, x * TILE_SIZE, y * TILE_SIZE, shadow);
}


void DrawGlyphAtMapTile (glyph_t *glyph, tile x, tile y, int shadow)
{
    DrawGlyph(glyph, x * TILE_SIZE + maprect.x, y * TILE_SIZE + maprect.y, shadow);
}
