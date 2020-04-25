//
//  obj.h
//  Azki
//
//  Created by Thomas Foster on 4/1/20.
//  Copyright © 2020 Thomas Foster. All rights reserved.
//

#ifndef obj_h
#define obj_h

#include <stdbool.h>
#include "glyph.h"

typedef enum
{
    DIR_EAST,
    DIR_NORTH,
    DIR_WEST,
    DIR_SOUTH,
    NUMDIRS
} dir_t;

typedef enum
{
    TYPE_NONE,
    TYPE_PLAYER,
    TYPE_TREE,
    TYPE_ROCK1,
    TYPE_ROCK2,
    TYPE_ROCK3,
    TYPE_ROCK4,
    TYPE_WATER,
    TYPE_GRASS1,
    TYPE_GRASS2,
    TYPE_GRASS3,
    TYPE_GRASS4,
    TYPE_SPIDER,
    TYPE_NESSIE,
    NUMTYPES,
    
    // objects with no objdef_t get A_Spawn'd
    TYPE_BULLET,
} objtype_t;

typedef enum
{
    OF_SOLID        = 0x01,
    OF_PUSHABLE     = 0x02,
    OF_NOEDITOR     = 0x04, // don't show glyph in editor
    OF_ENTITY       = 0x08, // add to object list
} objflags_t;

struct objdef_s;

typedef struct obj_s
{
    objtype_t   type;
    glyph_t     glyph;
    enum        { objst_remove, objst_active, objst_inactive } state;
    
    // object's current location (map tile)
    int         x, y;
    
    // object's speed
    int       dx, dy;
    
    // object's properties and stats
    int         flags;
    int         health;
    int         armor;
    
    // animation and timers
    int         tics;
    int         cooldown;
    
    void (* update)(struct obj_s *obj);
    void (* contact)(struct obj_s *obj1, struct obj_s *obj2);
    struct obj_s *next;
} obj_t;

// abstract definition of an object
typedef struct objdef_s
{
    glyph_t     glyph;
    objflags_t  flags;
    int         maxhealth;
    char        name[40]; // editor only
    
    void (* update)(struct obj_s *obj);
    void (* contact)(struct obj_s *obj1, struct obj_s *obj2);
} objdef_t;

extern obj_t *objlist;
extern objdef_t objdefs[];

int RunTimer (obj_t *obj);
bool TryMove (obj_t *obj, int x, int y);

obj_t   NewObjectFromDef (objtype_t type, int x, int y);
void DrawObject (obj_t *obj);

obj_t * List_AddObject (obj_t *add);
obj_t * List_RemoveObject (obj_t *rem);
void    List_RemoveAll (void);
void    List_DrawObjects (void);


#endif /* obj_h */
