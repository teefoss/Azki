//
//  video.c
//  Azki
//
//  Created by Thomas Foster on 3/30/20.
//  Copyright © 2020 Thomas Foster. All rights reserved.
//
//  SDL, graphics, and font

#include <stdbool.h>
#include "video.h"
#include "map.h"

SDL_Window *    window;
SDL_Renderer *  renderer;
SDL_Texture *   font_table;
SDL_Texture *   shadow_table;

int fgcolor = GRAY;
int bgcolor = BLACK;

SDL_Rect game_res = { 0, 0, 448, 280 };
SDL_Rect screen_res;
int windowed_scale = 1;
float fullscreen_scale = 1.0f;
bool fullscreen = false;

static bool video_started = false;

static const SDL_Color colors[] =
{
    {  28,  28,  30, 255 }, //  0 Black
    {   0,  64, 221, 255 }, //  1 Blue
    {  36, 138,  61, 255 }, //  2 Green
    {   0, 113, 164, 255 }, //  3 Cyan
    { 255,  69,  58, 255 }, //  4 Red
    { 191,  90, 242, 255 }, //  5 Magenta
    { 178,  80,   0, 255 }, //  6 Brown
    { 174, 174, 178, 255 }, //  7 White
    {  84,  84,  86, 255 }, //  8 Gray
    {  10, 132, 255, 255 }, //  9 Bright blue
    {  48, 209,  88, 255 }, // 10 Bright Green
    { 100, 210, 255, 255 }, // 11 Bright Cyan
    { 255, 100, 130, 255 }, // 12 Pink
    { 218, 143, 255, 255 }, // 13 Bright magenta
    { 255, 214,  10, 255 }, // 14 Yellow
    { 242, 242, 247, 255 }, // 15 Bright white
    {   0,   0,   0, 255 }, // 16 Pitch black
    { 255,   0, 255, 255 }  // 17 Transparent
};


static int frame_start;
static int frame_end;
static int dt;

void StartFrame (void)
{
    frame_start = SDL_GetTicks();
}

int LimitFrameRate (int fps)
{
    int ms_per_frame;
    
    ms_per_frame = roundf( 1000.0f / (float)fps );
    frame_end = SDL_GetTicks();
    
    dt = frame_end - frame_start;
    if (dt < ms_per_frame)
        SDL_Delay(ms_per_frame - dt);
    //else
        //printf("frame took %d ms!\n", dt);
    
    return dt;
}

void TextColor (int c)
{
    fgcolor = c;
}

void BackgroundColor (int c)
{
    bgcolor = c;
}

void SetPaletteColor (int c)
{
    SDL_SetRenderDrawColor(renderer, colors[c].r, colors[c].g, colors[c].b, 255);
}

void SetRGBColor (uint8_t r, uint8_t g, uint8_t b)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
}

void SetScale (int scl)
{
    if (scl < 1)
        return;
    
    windowed_scale = scl;
    SDL_RenderSetScale(renderer, windowed_scale, windowed_scale);
    SDL_SetWindowSize(window, game_res.w*windowed_scale, game_res.h*windowed_scale);
    printf("draw scale set to %d\n", windowed_scale);
}

void Clear (uint8_t r, uint8_t g, uint8_t b)
{
    SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
}

void Refresh (void)
{
    SDL_RenderPresent(renderer);
}

void FillRect (int x, int y, int w, int h)
{
    SDL_Rect r = { x, y, w, h };
    SDL_RenderFillRect(renderer, &r);
}



void PrintString (const char *s, pixel x, pixel y)
{
    glyph_t g;
    
    while (*s != '\0')
    {
        g.character = *s;
        g.fg_color = fgcolor;
        g.bg_color = TRANSP;
        DrawGlyph(&g, x, y, BLACK);
        x += TILE_SIZE;
        s++;
    }
}



void PrintChar (char c, int winx, int winy)
{
    glyph_t g;
    
    g.character = c;
    g.fg_color = fgcolor;
    g.bg_color = bgcolor;
    DrawGlyph(&g, winx, winy, TRANSP);
}




void ToggleFullscreen (void)
{
    int w, h;
    
    if (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN_DESKTOP)
    {
        SDL_SetWindowFullscreen(window, 0);
        SDL_RenderSetScale(renderer, windowed_scale, windowed_scale);
        UpdateDrawLocations(windowed_scale);
    }
    else
    {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        SDL_GetWindowSize(window, &w, &h);
        SDL_RenderSetScale(renderer, h / game_res.h, h / game_res.h);
        UpdateDrawLocations(h / game_res.h);
    }
}




//
//  UpdateDrawLocations
//  Center the maprect and HUD draw locations
//
void UpdateDrawLocations (float scl)
{
    if (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) {
        maprect.x = (screen_res.w/scl - maprect.w) / 2;
        maprect.y = (screen_res.h/scl - maprect.h) / 2;
    } else {
        maprect.x = (game_res.w - maprect.w) / 2;
        maprect.y = (game_res.h - maprect.h) / 2;
    }
    
    TopHUD.x = maprect.x;
    TopHUD.y = maprect.y - TILE_SIZE * 2;
    BottomHUD.x = maprect.x;
    BottomHUD.y = maprect.y + maprect.h + TILE_SIZE;
}






// Display message in lower right HUD
void LOG (const char *message, int color)
{
    int x = maprect.x + maprect.w - (int)strlen(message) * TILE_SIZE;
    TextColor(color);
    PrintString(message, x, BottomHUD.y);
}







void ShutdownVideo (void)
{
    if (!video_started)
        return;
    
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(font_table);
    SDL_DestroyTexture(shadow_table);
    
    SDL_QuitSubSystem(SDL_INIT_VIDEO);    
}




//
//  SetWindowScale
//  Figures out what's the window size will fit,
//  sets 'scale' and render scale, and window size
//  use an adjust value to decrease the scale
//
static void MaxWindowSize (int adjust)
{
    int scl;
    
    if (SDL_GetDisplayBounds(0, &screen_res) != 0)
        Error("SDL_GetDisplayBounds failed: %s");
    printf("desktop resolution: %dx%d\n", screen_res.w, screen_res.h);
    
    scl = 1;
    while (1)
    {
        if ((game_res.w * (scl + 1) < screen_res.w) &&
            (game_res.h * (scl + 1) < screen_res.h))
        {
            scl++;
        }
        else break;
    }
    SetScale(scl - adjust);
}


#define FONT_SIZE 8

static void CreateFontTable (void)
{
    uint8_t *      p, *p1;     // ptr to screen pixel
    int             clr, chr;
    const uint8_t * data;       // prt to font data
    int             y1, bit;
    SDL_Surface *   surface;
    const int       w = FONT_SIZE * 256;
    const int       h = FONT_SIZE * NUMCOLORS;
    extern const unsigned char fontdata[];
    
    surface = SDL_CreateRGBSurface(0, w, h, 8, 0, 0, 0, 0);
    SDL_LockSurface(surface);
    SDL_SetPaletteColors(surface->format->palette, colors, 0, NUMCOLORS);
    SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 255, 0, 255));
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 255, 0, 255));
            
    // draw font data to surface
    for (clr = 0; clr < NUMCOLORS; clr++) // NUMCOLOR rows of 256 chars
    {
        for (chr = 0; chr < 256; chr++)
        {
            data = &fontdata[chr * FONT_SIZE];
            p = surface->pixels + (clr * FONT_SIZE) * surface->pitch + (chr * FONT_SIZE);
            for (y1=0 ; y1<FONT_SIZE ; y1++, data++, p+=surface->pitch)
            {
                p1 = p;
                for (bit=FONT_SIZE-1 ; bit>=0 ; bit--, p1++) {
                    if ( *data & (1 << bit) ) {
                        *p1 = clr;
                    }
                }
            }
        }
    }
    SDL_UnlockSurface(surface);
    font_table = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}




void StartVideo (void)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        Error("Could not initialize SDL!");
    
    window = SDL_CreateWindow("Game", 0, 0, game_res.w, game_res.h, 0);
    if (!window)
        Error("Could not create game window!");
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE);
    if (!renderer)
        Error("Could not create game renderer!");
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    //MaxWindowSize(0); // TODO: uncomment
    SetScale(3);
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    
    // Handle MacOS Retina display
#if 0
    SDL_GetRendererOutputSize(renderer, &rw, &rh);
    if (rw != w || rh != h)
        SDL_RenderSetScale(renderer, rw / w, rh / h);
    else
        SDL_RenderSetScale(renderer, scale, scale);
#endif

    CreateFontTable();
    video_started = true;
}
