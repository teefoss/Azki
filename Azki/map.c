//
//  map.c
//  Azki
//
//  Created by Thomas Foster on 4/1/20.
//  Copyright © 2020 Thomas Foster. All rights reserved.
//

#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "map.h"
#include "video.h"

#define MAP_NAME_FMT "maps/%d.map"

map_t       map;
SDL_Rect    maprect;
SDL_Point   TopHUD;
SDL_Point   BottomHUD;

bool mapdirty = false;

char *mapnames[] =
{
    " ",
    "Forest Edge",
    "There is no level 2!",
    "Test Map",
};



//
// MapName
// returns the name for given map number
//
char *MapName (int mapnum)
{
    int numnames;

    numnames = sizeof(mapnames) / sizeof(mapnames[0]);
    if (mapnum > numnames)
        return NULL;
    return mapnames[mapnum];
}


//
// PrintMapName
// Print current map's name and number in Top HUD.
// Returns lenth of the entire printed string
//
int PrintMapName (void)
{
    char mapnum[4];
    int offset;
    char *name;
    
    snprintf(mapnum, 4, "%d", map.num);
    TextColor(WHITE);
    PrintString(mapnum, TopHUD.x, TopHUD.y);
    
    name = MapName(map.num);
    if (name)
    {
        offset = (int)strlen(mapnum) * TILE_SIZE + TILE_SIZE;
        TextColor(RED);
        PrintString(name, TopHUD.x + offset, TopHUD.y);
    }

    return (int)strlen(mapnum) + (int)strlen(name) + 1;
}



//
//  NextLevel
//  Go to next level by increment +1 or -1
//  keep looking for the next level if gap
//
void NextLevel (int incr)
{
    int next;
    
    next = map.num + incr;
    while (next > 0 && next <= 100)
    {
        printf("trying to load %d...\n", next);
        if ( LoadMap(next, &map) )
            return;
        else
            next += incr;
    }
}




//
//  LoadMap
//  Read map file data into 'map'
//
bool LoadMap (int mapnum, map_t * map)
{
    FILE *      file;
    char        filename[80];
    mapdata_t   mapdata;
    int         x, y;
    
    sprintf(filename, MAP_NAME_FMT, mapnum);
    printf("Loading map %d...\n", mapnum);
    file = fopen(filename, "rb");
    if (!file)
    {
        fclose(file);
        printf("LoadFile: Quit, couldn't load %s\n", filename);
        return false;
    }
    
    fread(&mapdata, sizeof(mapdata_t), 1, file);
    fclose(file);
    
    for (y=0 ; y<MAP_H ; y++)
    {
        for (x=0 ; x<MAP_W ; x++)
        {
            map->background[y][x] = NewObjectFromDef(mapdata.background[y][x], x, y);
            map->foreground[y][x] = NewObjectFromDef(mapdata.foreground[y][x], x, y);
        }
    }
    
    map->num = mapnum;
    mapdirty = false;
    
    return true;
}




//
//  SaveMap
//  Save map to file
//
bool SaveMap (map_t * map)
{
    FILE        *stream;
    char        filename[80];
    int         x, y;
    mapdata_t   mapdata;
    
    sprintf(filename, MAP_NAME_FMT, map->num);
    stream = fopen(filename, "wb");
    if (!stream)
    {
        fclose(stream);
        printf("SaveMap: Quit, couldn't open %s!\n", filename);
        return false;
    }
    
    for (y=0 ; y<MAP_H ; y++)
    {
        for (x=0 ; x<MAP_W ; x++)
        {
            mapdata.background[y][x] = map->background[y][x].type;
            mapdata.foreground[y][x] = map->foreground[y][x].type;
        }
    }
    
    fwrite(&mapdata, sizeof(mapdata), 1, stream);
    fclose(stream);
    printf("SaveMap: saved %s\n", filename);
    
    mapdirty = false;
    
    return true;
}




//
//  NewMap
//  Create a new file and blank map
//
bool NewMap (int num, map_t * map)
{
    FILE        *stream;
    int         x, y;
    char        filename[80];
    mapdata_t   mapdata;
    
    sprintf(filename, MAP_NAME_FMT, num);
    printf("New map, creating %s...\n", filename);
    
    stream = fopen(filename, "wb");
    if (!stream) {
        printf("NewMap: could not create file %s!\n", filename);
        return false;
    }

    map->num = num;
    
    // empty map
    for (y=0 ; y<MAP_H ; y++)
    {
        for (x=0 ; x<MAP_W ; x++)
        {
            map->foreground[y][x] = NewObjectFromDef(TYPE_NONE, x, y);
            map->background[y][x] = NewObjectFromDef(TYPE_NONE, x, y);
            mapdata.foreground[y][x] = TYPE_NONE;
            mapdata.background[y][x] = TYPE_NONE;
        }
    }
    
    if ( fwrite(&mapdata, sizeof(mapdata), 1, stream) != 1 ) {
        printf("NewMap: could not write map to file %s!\n", filename);
        return false;
    }
    fclose(stream);
    
    mapdirty = false;
    
    return true;
}




void DrawMap (map_t *map)
{
    const int   margin = 3;
    
    obj_t *     obj;
    obj_t *     bkg;
    int         w, h;
    int         i;
    
    // draw map background and shadow
    w = maprect.w + (margin * 2);
    h = maprect.h + (margin * 2);
    SetRGBColor(14, 14, 15);
    FillRect(maprect.x - margin + 2, maprect.y - margin + 2, w, h);
    SetPaletteColor(BLACK);
    FillRect(maprect.x - margin, maprect.y - margin, w, h);
    
    // draw all objects
    obj = &map->foreground[0][0];
    bkg = &map->background[0][0];
    for (i=0 ; i<MAP_W*MAP_H ; i++)
    {
        if (state == STATE_PLAY)
        {
            DrawObject(bkg++);
            DrawObject(obj++);
        }
        else if (state == STATE_EDIT)
        {
            if (showlayer == LAYER_BG || showlayer == LAYER_BOTH)
                DrawObject(bkg++);
            if (showlayer == LAYER_FG || showlayer == LAYER_BOTH)
                DrawObject(obj++);
        }
    }
}
