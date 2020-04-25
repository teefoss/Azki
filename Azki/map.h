//
//  map.h
//  Azki
//
//  Created by Thomas Foster on 4/1/20.
//  Copyright © 2020 Thomas Foster. All rights reserved.
//

#ifndef map_h
#define map_h

#include <stdbool.h>
#include <SDL2/SDL.h>
#import "obj.h"

#define TILE_SIZE       8       // tiles are 8 x 8 pixels
#define MAP_TOP_MARGIN  3 * TILE_SIZE
#define MAP_SIDE_MARGIN 2 * TILE_SIZE
#define MAP_W           52
#define MAP_H           29
#define MAP_NAME_LEN    MAP_W

typedef struct map_s
{
    int num;
    obj_t foreground[MAP_H][MAP_W];
    obj_t background[MAP_H][MAP_W];
} map_t;

typedef struct
{
    objtype_t foreground[MAP_H][MAP_W];
    objtype_t background[MAP_H][MAP_W];
} mapdata_t;

extern char filename[80];
extern SDL_Rect maprect;
extern SDL_Point TopHUD;
extern SDL_Point BottomHUD;
extern bool mapdirty;

int PrintMapName (void);
void NextLevel (int incr);

bool LoadMap (int mapnum, map_t * map);
bool NewMap (int num, map_t * map);
bool SaveMap (map_t * map);

void DrawMap (map_t *map);
char *MapName (int mapnum);

#endif /* map_h */
