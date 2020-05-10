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
#include "azki.h"

typedef enum
{
    DIR_NONE,
    DIR_EAST,
    DIR_NORTH,
    DIR_WEST,
    DIR_SOUTH
} dir_t;

typedef enum
{
    // layer types - appear in editor
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
    TYPE_STONE1,
    TYPE_STONE2,
    TYPE_DIRT,
    TYPE_SPIDER,
    TYPE_NESSIE,
    TYPE_ORGE,
    TYPE_CORPSE,
    TYPE_GOLDKEY,
    TYPE_BLUEKEY,
    TYPE_GREENKEY,
    TYPE_GOLDDOOR,
    TYPE_BLUEDOOR,
    TYPE_GREENDOOR,
    TYPE_SWITCH,
    TYPE_BLOCK,
    TYPE_HEART,
    TYPE_BOAT,
    TYPE_EXIT,
    NUMLAYERTYPES,
    
    // misc objects that don't appear in editor (projectiles etc.)
    TYPE_PROJ_BALL,
    TYPE_PROJ_RING,
    NUMTYPES
} objtype_t;

typedef enum
{
    OF_SOLID        = 0x0001,
    // player can move it
    OF_PUSHABLE     = 0x0002,
    // don't show glyph in editor
    OF_NOEDITOR     = 0x0004,
    // add to object list, all objects that need to update() and contact()
    OF_ENTITY       = 0x0008,
    // some entities will walk into water
    OF_CANDROWN     = 0x0010,
    // collectible by player
    OF_ITEM         = 0x0020,
    OF_BREAKABLE    = 0x0040,
    // inflicts damage on player
    OF_DAMAGING     = 0x0080,
} objflags_t;

struct objdef_s;
struct obj_s;

typedef void (* action1_t)(struct obj_s *);
typedef void (* action2_t)(struct obj_s *, struct obj_s *);

typedef struct obj_s
{
    objtype_t   type;
    glyph_t     glyph;
    enum        { objst_remove, objst_active, objst_inactive } state;
    
    // object's definitition (objdefs[obj->type])
    struct objdef_s * info;
    
    // object's current location (map tile)
    // this will be used as an interger value most of the time
    int         x;
    int         y;
    
    // object's speed
    int         dx;
    int         dy;
    
    // object's properties and stats
    int         flags;
    int         hp;     // hit points (health or damage)
    int         armor;
    
    // animation and timers
    int         tics;
    int         updatedelay; // how many frames to wait
    int         hittimer;
    
    // update and contact functions, called every frame
    action1_t   update;
    action2_t   contact;
    
    // who created this object, e.g. projectiles
    // player: the thing that killed the player
    struct obj_s *src;
    
    // object's target, e.g. projectiles
    struct obj_s *dst;
    
    // linked list
    struct obj_s *next;
} obj_t;

// abstract definition of an object
typedef struct objdef_s
{
    glyph_t     glyph;
    objflags_t  flags;
    int         maxhealth;
    char        name[40]; // editor only
    char        hud[40]; // in-game info
    
    action1_t   update;
    action2_t   contact;
} objdef_t;

extern obj_t *objlist;
extern objdef_t objdefs[];

const char *    ObjName (obj_t *obj);
const char *    ObjectNameAtXY (tile x, tile y);

int         RunTimer (obj_t *obj);
bool        TryMove (obj_t *obj, tile x, tile y);
objtype_t   ObjectTypeAtXY (tile x, tile y);
glyph_t *   ObjectGlyphAtXY (tile x, tile y);
bool        ObjectsOverlap (obj_t *obj1, obj_t *obj2);
void        RemoveObj (obj_t *obj);
void        FlashObject (obj_t *obj, int *timer, int color);
void DamageObj (obj_t *src, obj_t *target, int damage);
int ObjectDistance (obj_t *obj1, obj_t *obj2);

obj_t NewObjectFromDef (objtype_t type, tile x, tile y);
void ChangeObject (obj_t *obj, objtype_t type, int state);
void DrawObject (obj_t *obj);

obj_t *     List_AddObject (obj_t *add);
obj_t *     List_RemoveObject (obj_t *rem);
void        List_RemoveAll (void);
int         List_Count (void);
void        List_DrawObjects (void);
objtype_t   List_ObjectAtXY (tile x, tile y);


#endif /* obj_h */
