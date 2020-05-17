//
//  editor.c
//  Azki
//
//  Created by Thomas Foster on 4/2/20.
//  Copyright © 2020 Thomas Foster. All rights reserved.
//

#include <SDL2/SDL.h>
#include <string.h>

#include "azki.h"
#include "video.h"
#include "obj.h"
#include "map.h"

typedef enum {
    LAYER_FG,
    LAYER_BG,
    LAYER_BOTH
} layerview_t;


// object selection grid
typedef struct
{
    bool shown;
    int rows;
    int cols;
    SDL_Rect rect;
} grid_t;

static grid_t grid;
static objtype_t cursor = TYPE_PLAYER;
static char lowermsg[100];
static layerview_t activelayer;
static layerview_t viewlayer;

static char *layer_msg[] = {
    "(Editing Foreground)",
    "(Editing Background)"
};
static char *show_msg[] = {
    "Showing: FG   ",
    "Showing:    BG",
    "Showing: FG BG"
};

control_t editorcontrols[] =
{
    { " ", " " },
    { " ", " "},
    { "KEY", "ACTION" },
    { "--------", "--------" },
    { "`", "Save and play level" },
    { "+ and -", "Increase/Decrease window size" },
    { "[ and ]", "Save and previous/next level" },
    { "F1", "Show this screen!" },
    { "F2", "Show Character Viewer"},
    { "CTRL-S", "Save map" },
    { "ESCAPE", "Save and Quit" },
    { "--------", "--------" },
    { "TAB", "Show Object Palette" },
    { "L MOUSE", "Place object" },
    { "R MOUSE", "\"Pick up\" object" },
    { "SPACE", "Switch layer" },
    { "F", "Show foreground layer only" },
    { "S", "Show background layer only" },
    { "stop", "stop" }
};





//
//  MakeSelectionGrid
//  Initialize the selection grid
//
void MakeSelectionGrid ()
{
    grid.cols = game_res.w / TILE_SIZE;
    grid.rows = NUMLAYERTYPES / grid.cols + 1;
    grid.rect.x = 0;
    grid.rect.y = game_res.h - grid.rows * TILE_SIZE;
    grid.rect.w = game_res.w;
    grid.rect.h = grid.rows * TILE_SIZE;
}


obj_t *ObjectAtMapTile (SDL_Point *mousetile)
{
    return activelayer == LAYER_FG
        ? &map.foreground[mousetile->y][mousetile->x]
        : &map.background[mousetile->y][mousetile->x];
}



#pragma mark - Grid

objtype_t TypeAtGridTile (SDL_Point * mousept)
{
    int type;
    int x, y;
    
    x = mousept->x / TILE_SIZE;
    y = (mousept->y / TILE_SIZE) - (game_res.h / TILE_SIZE - grid.rows);

    type = x + y * grid.cols;
    
    if (type >= NUMLAYERTYPES)   // if clicked on an empty tile,
        return cursor;      // don't change the current selection
    return type;
}


SDL_Point GridTileForType (objtype_t type)
{
    SDL_Point tile = { type % grid.cols, type / grid.cols};
    return tile;
}



#pragma mark - Operations

void EditorSaveMap (void)
{
    if ( SaveMap(&map) )
        strcpy(lowermsg, "Map saved!");
    else
        strcpy(lowermsg, "Error saving map!");
}



void FloodFill (tile x, tile y, objtype_t oldtype, objtype_t newtype)
{
    if (oldtype == newtype)
        return;
    
    if (x < 0 || x >= MAP_W || y < 0 || y >= MAP_H)
        return;
    
    switch (activelayer) {
        case LAYER_FG:
            if (map.foreground[y][x].type != oldtype)
                return;
            map.foreground[y][x] = NewObjectFromDef(newtype, x, y);
            break;
        case LAYER_BG:
            if (map.background[y][x].type != oldtype)
                return;
            map.background[y][x] = NewObjectFromDef(newtype, x, y);
            break;
        default:
            break;
    }
    
    FloodFill(x, y + 1, oldtype, newtype);
    FloodFill(x, y - 1, oldtype, newtype);
    FloodFill(x - 1, y, oldtype, newtype);
    FloodFill(x + 1, y, oldtype, newtype);
}



#pragma mark - Drawing

//
//  DrawSelectionGrid
//  The grid is shown, draw it and the HUD info.
//
void DrawSelectionGrid (SDL_Point * mousept)
{
    int type;
    int x, y;
    SDL_Point mousetile;
    SDL_Rect selbox;
    extern objdef_t objdefs[];
    
    // background
    SDL_SetRenderDrawColor(renderer, 14, 14, 14, 255);
    SDL_RenderFillRect(renderer, &grid.rect);
    
    // draw all editor object types in the selection grid
    type = 0;
    x = 0;
    y = grid.rect.y;
    for (type=0 ; type<NUMLAYERTYPES ; type++, x+=TILE_SIZE )
    {
        //type = objtypes[i];
        
        if ( x >= game_res.w )
        {
            x = 0;
            y += TILE_SIZE;
        }
        if ( type == TYPE_NONE )
            DrawGlyph(&(glyph_t){'E',RED,TRANSP}, x, y, TRANSP);
        else
            DrawGlyph(&objdefs[type].glyph, x, y, TRANSP);
        // TODO: handle when a glyph color is the same as the bkg
    }
    
    // draw selection box
    if ( SDL_PointInRect(mousept, &grid.rect) )
    {
        mousetile.x = mousept->x / TILE_SIZE;
        mousetile.y = mousept->y / TILE_SIZE;
        selbox = (SDL_Rect){
            mousetile.x * TILE_SIZE,
            mousetile.y * TILE_SIZE,
            TILE_SIZE,
            TILE_SIZE
        };
        
        // HUD
        objtype_t hover = TypeAtGridTile(mousept);
        TextColor(BRIGHTGREEN);
        PrintString(objdefs[hover].name, TopHUD.x, TopHUD.y);
    }
    else // show current selection
    {
#if 0
        objtype_t *ot;
     
        // can't use cursor type - look it up:
        ot = objtypes;
        while ( *ot != cursor)
            ot++;
#endif
        
        SDL_Point selected_tile = GridTileForType(cursor);
        selbox = (SDL_Rect){
            selected_tile.x * TILE_SIZE,
            (selected_tile.y * TILE_SIZE) + grid.rect.y,
            TILE_SIZE,
            TILE_SIZE,
        };

        // HUD
        TextColor(BRIGHTGREEN);
        PrintString(objdefs[cursor].name, TopHUD.x, TopHUD.y);
    }
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &selbox);
}



void DrawCursor (SDL_Point *mousetile)
{
    int sh;
    
    // display a helpful box so we know we're editing the bg
    SDL_RenderSetViewport(renderer, &maprect);
    if (activelayer == LAYER_BG)
    {
        SDL_Rect helpful = {
            mousetile->x * TILE_SIZE - 2,
            mousetile->y * TILE_SIZE - 2,
            TILE_SIZE + 4,
            TILE_SIZE + 4
        };
        SetPaletteColor(BROWN);
        SDL_RenderDrawRect(renderer, &helpful);
    }
    
    // draw cursor
    if (cursor == TYPE_NONE || keys[SDL_SCANCODE_D])
    {
        SDL_Rect box = {
            mousetile->x * TILE_SIZE,
            mousetile->y * TILE_SIZE,
            TILE_SIZE,
            TILE_SIZE
        };
        if (keys[SDL_SCANCODE_D])
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        else
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &box);
    }
    else
    {
        sh = (SDL_GetTicks() % 600) < 300 ? RED : BLACK; // flip shadow
        DrawGlyph(&objdefs[cursor].glyph,
                  mousetile->x*TILE_SIZE,
                  mousetile->y*TILE_SIZE,
                  sh);
    }
    SDL_RenderSetViewport(renderer, NULL);
    
}


void DrawEditorHUD (SDL_Point *mousept, SDL_Point *mousetile)
{
    int msg_x;
    char mouseinfo[100];
    
    // layer
    msg_x = TopHUD.x + maprect.w - (int)strlen(layer_msg[activelayer]) * TILE_SIZE;
    TextColor(activelayer == LAYER_FG ? YELLOW : BROWN);
    PrintString(layer_msg[activelayer], msg_x, TopHUD.y);
                
    // lower HUD
    TextColor(MAGENTA);
    PrintString(lowermsg, BottomHUD.x, BottomHUD.y);
    
    // print mouse map coordinates
    if (SDL_PointInRect(mousept, &maprect))
        sprintf(mouseinfo, "%s (%2d, %2d)", show_msg[viewlayer], mousetile->x, mousetile->y);
    else
        sprintf(mouseinfo, "%s (--, --)", show_msg[viewlayer]);
    LOG(mouseinfo, BRIGHTWHITE);
}



void EditorDrawMap (map_t *map)
{
    obj_t *fg, *bg;
    int i;
    void DrawMapBackground(void);
    
    DrawMapBackground();

    fg = &map->foreground[0][0];
    bg = &map->background[0][0];
    for (i=0 ; i<MAP_W*MAP_H ; i++)
    {
        if (viewlayer == LAYER_BG || viewlayer == LAYER_BOTH)
            DrawObject(bg++);
        if (viewlayer == LAYER_FG || viewlayer == LAYER_BOTH)
            DrawObject(fg++);
    }
}






#pragma mark - Input

void EditorKeyDown (SDL_Keycode key)
{
    memset(lowermsg, 0, sizeof(lowermsg)); // clear the LOG message
    
    switch (key)
    {
        case SDLK_ESCAPE:
            EditorSaveMap();
            Quit(NULL);
            break;
            
        // window scale
        case SDLK_MINUS:
            SetScale(windowed_scale - 1);
            break;
        case SDLK_EQUALS:
            SetScale(windowed_scale + 1);
            break;
            
        // fullscreen
        case SDLK_f:
            if (CTRL) ToggleFullscreen();
            break;
        case SDLK_BACKSLASH:
            ToggleFullscreen();
            break;
            
        // level select
        case SDLK_LEFTBRACKET:
            SaveMap(&map);
            NextLevel(-1);
            break;
        case SDLK_RIGHTBRACKET:
            SaveMap(&map);
            NextLevel(+1);
            break;
            
        // switch layer
        case SDLK_SPACE:
            activelayer ^= 1;
            break;
            
        // switch to erase
        case SDLK_x:
            cursor = TYPE_NONE;
            break;
            
        // save map
        case SDLK_s:
            if (CTRL)
                EditorSaveMap();
            break;
            
        // switch to play
        case SDLK_BACKQUOTE:
            SaveMap(&map);
            state = STATE_PLAY;
            break;
            
        case SDLK_F1:
            S_Controls("EDITOR CONTROLS", editorcontrols);
            break;
            
        case SDLK_F2:
            S_CharacterViewer();
            break;
            
        default:
            break;
    }
}




void EditorMouseDown (SDL_Point * mousept, SDL_Point * mousetile)
{
    obj_t *obj;
    
    // place an object on map if editing
    if (!grid.shown && SDL_PointInRect(mousept, &maprect))
    {
        obj = ObjectAtMapTile(mousetile);
        if (keys[SDL_SCANCODE_F]) {
            FloodFill(mousetile->x, mousetile->y, obj->type, cursor);
        }
//        else if (keys[SDL_SCANCODE_D]) {
//            cursor = obj->type;
//        }
        else {
            *obj = NewObjectFromDef(cursor, mousetile->x, mousetile->y);
        }
        
        mapdirty = true;
    }
    
    // select an object if grid is open
    if (grid.shown && SDL_PointInRect(mousept, &grid.rect))
    {
        cursor = TypeAtGridTile(mousept);
    }
}


#pragma mark -

void EditorLoop (void)
{
    SDL_Event   event;
    uint32_t    mousestate;
    SDL_Point   mousept;
    SDL_Point   mousetile;
    
    memset(lowermsg, 0, sizeof(lowermsg));
    MakeSelectionGrid();
    activelayer = LAYER_FG;
    LoadMap(map.num, &map); // entities were removed in play, reload
        
    while (state == STATE_EDIT)
    {
        StartFrame();
        
        mousestate = SDL_GetMouseState(&mousept.x, &mousept.y);
        mousept.x /= windowed_scale; // TODO: fix for just current scale
        mousept.y /= windowed_scale;
        mousetile.x = (mousept.x - maprect.x) / TILE_SIZE;
        mousetile.y = (mousept.y - maprect.y) / TILE_SIZE;

        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    Quit(NULL);
                    break;
                case SDL_KEYDOWN:
                    EditorKeyDown(event.key.keysym.sym);
                    if (state == STATE_PLAY)
                        return;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    switch (event.button.button) {
                        case SDL_BUTTON_RIGHT:
                        {
                            obj_t *obj = ObjectAtMapTile(&mousetile);
                            cursor = obj->type;
                        } break;
                            
                        default:
                            break;
                    }
                default:
                    break;
            }
        }
        
        grid.shown = keys[SDL_SCANCODE_TAB];
        
        if (keys[SDL_SCANCODE_F])
            viewlayer = LAYER_FG;
        else if (keys[SDL_SCANCODE_B])
            viewlayer = LAYER_BG;
        else
            viewlayer = LAYER_BOTH;
        
        if (mousestate & SDL_BUTTON_LMASK)
            EditorMouseDown(&mousept, &mousetile);
        
        Clear(0, 0, 0);
        
        EditorDrawMap(&map);
        DrawEditorHUD(&mousept, &mousetile);
        
        if ( SDL_PointInRect(&mousept, &maprect) )
            DrawCursor(&mousetile);
        if ( grid.shown )
            DrawSelectionGrid(&mousept);
        else
            PrintMapName();
        
        Refresh();
        LimitFrameRate(FRAME_RATE);
    }
}
