//
//  obj.c
//  Azki
//
//  Created by Thomas Foster on 4/1/20.
//  Copyright © 2020 Thomas Foster. All rights reserved.
//

#include "obj.h"
#include "player.h"
#include "video.h"
#include "action.h"

// linked list of active (mobile) entities
obj_t *objlist;

// { flags, maxhealth, update }
objdef_t objdefs[NUMTYPES] =
{
    {   // TYPE_NONE,
        .glyph = { CHAR_NUL, TRANSP, TRANSP },
        .flags = 0,
        .maxhealth = 0,
        .name = "None",
        .update = NULL
    },
    {   // TYPE_PLAYER,
        .glyph = { CHAR_FACE_SOLID, BRIGHTBLUE, TRANSP },
        .flags = OF_ENTITY,
        .maxhealth = 100,
        .name = "Player",
        .update = UpdatePlayer
    },
    {   // TYPE_TREE
        .glyph = { CHAR_CLUB, GREEN, TRANSP },
        .flags = OF_SOLID,
        .maxhealth = 0,
        .name = "Tree",
        .update = NULL
    },
    
    {   // TYPE_ROCK1,
        .glyph = { 177, GRAY, BLACK },
        .flags = OF_SOLID,
        .maxhealth = 0,
        .name = "Rock 1",
        .update = NULL
    },
    {   // TYPE_ROCK2,
        .glyph = { 177, WHITE, BLACK },
        .flags = OF_SOLID,
        .maxhealth = 0,
        .name = "Rock 2",
        .update = NULL
    },
    {   // TYPE_ROCK3,
        .glyph = { 178, GRAY, BLACK },
        .flags = OF_SOLID,
        .maxhealth = 0,
        .name = "Rock 3",
        .update = NULL
    },
    {   // TYPE_ROCK4,
        .glyph = { 178, WHITE, BLACK },
        .flags = OF_SOLID,
        .maxhealth = 0,
        .name = "Rock 4",
        .update = NULL
    },

    {   // TYPE_WATER
        .glyph = { CHAR_NUL, BRIGHTBLUE, BLUE },
        .flags = 0,
        .maxhealth = 0,
        .name = "Water",
        .update = A_UpdateWater
    },
    
    {   // TYPE_GRASS
        .glyph = { 176, GREEN, TRANSP },
        .flags = 0,
        .maxhealth = 0,
        .name = "Grass",
        .update = NULL
    },
};




obj_t *
List_AddObject (obj_t *add)
{
    obj_t *new;
    
    new = malloc(sizeof(obj_t));
    *new = *add;
    
    if (objlist)
    {
        objlist = new;
        new->next = NULL;
    }
    else
    {
        new->next = objlist;
        objlist = new;
    }
    return new;
}


void
List_RemoveObject (obj_t *rem)
{
    obj_t * prev;
    
    // the first and only
    if (rem == objlist && !rem->next)
        objlist = NULL;
    
    // the first of multiple
    else if (rem == objlist && rem->next)
        objlist = rem->next;

    // it's somewhere down the list
    else
    {
        // find the one prior
        prev = objlist;
        while (prev->next != rem)
            prev = prev->next;

        // at the end
        if (!rem->next)
            prev->next = NULL;
        else // somewhere in the middle
            prev->next = rem->next;
    }
    free(rem);
    rem = NULL;
}



void
List_RemoveAll (void)
{
    obj_t * obj;
    obj_t * temp;
    
    if (!objlist)
        return;
    
    obj = objlist;
    while (obj) {
        temp = obj;
        obj = obj->next;
        free(temp);
    };
    objlist = NULL;
}



void List_DrawObjects (void)
{
    obj_t *obj;
    int draw_x, draw_y;

    if (!objlist)
        return;
    
    obj = objlist;
    do {
        if (obj->type != TYPE_NONE)
        {
            draw_x = obj->x * TILE_SIZE + maprect.x;
            draw_y = obj->y * TILE_SIZE + maprect.y;
            DrawGlyph(&obj->glyph, draw_x, draw_y, PITCHBLACK);
        }
        obj = obj->next;
    } while (obj);
}




void DrawObject (obj_t *obj)
{
    int winx, winy;
    
    if (obj->type == TYPE_NONE)
        return; // don't bother
    
    if ((obj->flags & OF_ENTITY) && state == GS_PLAY)
        return;
    
    if ( !(obj->flags & OF_ENTITY) && obj->update)
        obj->update(obj); // update any fg/bg objects
        
    winx = obj->x * TILE_SIZE + maprect.x; // draw loctions
    winy = obj->y * TILE_SIZE + maprect.y;
    DrawGlyph( &obj->glyph, winx, winy, PITCHBLACK );
}


//
//  NewObject
//  Initialize a new object from its objdef_t at (x, y)
//
obj_t NewObject (objtype_t type, int x, int y)
{
    obj_t new;
    
    memset(&new, 0, sizeof(new));
    
    new.type = type;
    new.x = x;
    new.y = y;
    
    // initial default definition values
    new.glyph = objdefs[type].glyph;
    new.health = objdefs[type].maxhealth;
    new.flags = objdefs[type].flags;
    new.update = objdefs[type].update;
    new.contact = NULL; // TODO: add contact functions
    
    return new;
}
