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
#include "map.h"

// singly linked list of active (mobile) entities
obj_t *objlist;

objdef_t objdefs[NUMTYPES] =
{
    {   // TYPE_NONE,
        .glyph = { CHAR_NUL, TRANSP, TRANSP },
        .flags = 0,
        .maxhealth = 0,
        .name = "None",
        .update = NULL,
        .contact = NULL,
    },
    {   // TYPE_PLAYER,
        .glyph = { CHAR_FACE_SOLID, BRIGHTBLUE, TRANSP },
        .flags = OF_ENTITY,
        .maxhealth = 100,
        .name = "Player",
        .update = P_UpdatePlayer,
        .contact = P_PlayerContact
    },
    {   // TYPE_TREE
        .glyph = { CHAR_CLUB, GREEN, TRANSP },
        .flags = OF_SOLID,
        .maxhealth = 0,
        .name = "Tree",
        .update = NULL,
        .contact = NULL,
    },
    
    {   // TYPE_ROCK1,
        .glyph = { 177, GRAY, BLACK },
        .flags = OF_SOLID,
        .maxhealth = 0,
        .name = "Rock 1",
        .update = NULL,
        .contact = NULL,
    },
    {   // TYPE_ROCK2,
        .glyph = { 177, WHITE, BLACK },
        .flags = OF_SOLID,
        .maxhealth = 0,
        .name = "Rock 2",
        .update = NULL,
        .contact = NULL,
    },
    {   // TYPE_ROCK3,
        .glyph = { 178, GRAY, BLACK },
        .flags = OF_SOLID,
        .maxhealth = 0,
        .name = "Rock 3",
        .update = NULL,
        .contact = NULL,
    },
    {   // TYPE_ROCK4,
        .glyph = { 178, WHITE, BLACK },
        .flags = OF_SOLID,
        .maxhealth = 0,
        .name = "Rock 4",
        .update = NULL,
        .contact = NULL,
    },

    {   // TYPE_WATER
        .glyph = { CHAR_NUL, BRIGHTBLUE, BLUE },
        .flags = 0,
        .maxhealth = 0,
        .name = "Water",
        .update = A_UpdateWater,
        .contact = NULL
    },
    
    {   // TYPE_GRASS1
        .glyph = { 176, GREEN, TRANSP },
        .flags = 0,
        .maxhealth = 0,
        .name = "Grass 1",
        .update = NULL,
        .contact = NULL
    },
    {   // TYPE_GRASS2
        .glyph = { 176, BRIGHTGREEN, TRANSP },
        .flags = 0,
        .maxhealth = 0,
        .name = "Grass 2",
        .update = NULL,
        .contact = NULL
    },
    {   // TYPE_GRASS3
        .glyph = { 177, GREEN, TRANSP },
        .flags = 0,
        .maxhealth = 0,
        .name = "Grass 3",
        .update = NULL,
        .contact = NULL
    },
    {   // TYPE_GRASS4
        .glyph = { 177, BRIGHTGREEN, TRANSP },
        .flags = 0,
        .maxhealth = 0,
        .name = "Grass 4",
        .update = NULL,
        .contact = NULL
    },


    
    {   // TYPE_SPIDER
        .glyph = { '*', GRAY, TRANSP },
        .flags = OF_ENTITY,
        .maxhealth = 20,
        .name = "Spider",
        .update = A_UpdateSpider,
        .contact = NULL
    },
        
    {   // TYPE_NESSIE
        .glyph = { 244, BRIGHTGREEN, TRANSP },
        .flags = OF_ENTITY,
        .maxhealth = 40,
        .name = "Nessie",
        .update = A_NessieUpdate,
        .contact = NULL
    },

    {   // NUMTYPES (placeholder)
        .glyph = { CHAR_NUL, TRANSP, TRANSP },0,0,"",NULL,NULL
    },
    
    //==========================================================================

    {   // TYPE_PROJ_BALL
        .glyph = { CHAR_DOT1, YELLOW, TRANSP },
        .flags = OF_ENTITY,
        .maxhealth = 10,
        .name = "Ball Projectile",
        .update = A_UpdateProjectile,
        .contact = A_ProjectileContact
    },
    
    {   // TYPE_PROJ_RING
        .glyph = { 9, MAGENTA, TRANSP },
        .flags = OF_ENTITY,
        .maxhealth = 0,
        .name = "Ring Projectile",
        .update = A_UpdateProjectile,
        .contact = A_ProjectileContact
    },
};



int RunTimer (obj_t *obj)
{
    if (obj->tics)
        obj->tics--;
    return obj->tics;
}



bool TryMove (obj_t *obj, float x, float y)
{
    // keep object within map
    if ( x < 0 || x >= MAP_W || y < 0 || y >= MAP_H )
        return false;

    // move if dest is not solid
    if ( !(map.foreground[(int)y][(int)x].flags & OF_SOLID) )
    {
        obj->x = x;
        obj->y = y;
        return true;
    }
    
    return false;
}




obj_t *
List_AddObject (obj_t *add)
{
    obj_t *new;
    
    printf("adding object: %d\n", add->type);
    new = malloc(sizeof(obj_t));
    if (!new)
        Error("List_AddObject: error, could not alloc mem");
    *new = *add;
    
    new->state = objst_active;
    new->next = objlist;
    objlist = new;
        
    return new;
}


//
// List_RemoveObject
// Remove object and return the next object in the list
//
obj_t *
List_RemoveObject (obj_t *rem)
{
    obj_t * prev;
    obj_t * ret;
    
    if (!rem)
        Error("List_RemoveObject: error, tried to remove NULL object!");
    printf("removed type %d\n", rem->type);
    
    // the first and only
    if (rem == objlist && !rem->next)
    {
        objlist = NULL;
        ret = NULL;
    }
    
    // the first of multiple
    else if (rem == objlist && rem->next)
    {
        objlist = rem->next;
        ret = rem->next;
    }

    // it's somewhere down the list
    else
    {
        // find the one prior
        prev = objlist;
        while (prev->next != rem)
            prev = prev->next;

        // at the end
        if (!rem->next)
        {
            prev->next = NULL;
            ret = NULL;
        }
        else // somewhere in the middle
        {
            prev->next = rem->next;
            ret = rem->next;
        }
    }
    free(rem);
    rem = NULL;
    return ret;
}



void
List_RemoveAll (void)
{
    obj_t * obj;
    obj_t * temp;
    int i;
    
    if (!objlist)
        return;
    
    i = 0;
    obj = objlist;
    while (obj) {
        temp = obj;
        obj = obj->next;
        free(temp);
        i++;
    };
    objlist = NULL;
    printf("List_RemoveAll: removed %i objects\n", i);
}



#define draw_x(n)   (n * TILE_SIZE + maprect.x)
#define draw_y(n)   (n * TILE_SIZE + maprect.y)

void List_DrawObjects (void)
{
    obj_t *obj;

    if (!objlist)
        return;
    
    obj = objlist;
    do {
        if (obj->type != TYPE_NONE)
            DrawGlyph(&obj->glyph, draw_x(obj->x), draw_y(obj->y), PITCHBLACK);
        obj = obj->next;
    } while (obj);
}




void DrawObject (obj_t *obj)
{
    int winx, winy;
    
    if (obj->type == TYPE_NONE)
        return; // don't bother
    
    if (obj->update)
        obj->update(obj); // update any fg/bg objects
        
    winx = obj->x * TILE_SIZE + maprect.x; // draw loctions
    winy = obj->y * TILE_SIZE + maprect.y;
    DrawGlyph( &obj->glyph, winx, winy, PITCHBLACK );
}


//
//  NewObject
//  Initialize a new object from its objdef_t at (x, y)
//
obj_t NewObjectFromDef (objtype_t type, tile x, tile y)
{
    obj_t new;
    
    memset(&new, 0, sizeof(new));
    
    new.type = type;
    new.x = x;
    new.y = y;
    
    // initial default definition values
    new.glyph = objdefs[type].glyph;
    new.hp = objdefs[type].maxhealth;
    new.flags = objdefs[type].flags;
    new.update = objdefs[type].update;
    new.contact = objdefs[type].contact;
    
    return new;
}
