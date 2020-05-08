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
        .hud = "",
        .update = NULL,
        .contact = NULL,
    },
    {   // TYPE_PLAYER,
        .glyph = { CHAR_FACE_SOLID, BRIGHTBLUE, TRANSP },
        .flags = OF_ENTITY|OF_SOLID,
        .maxhealth = 3,
        .name = "Player",
        .hud = "",
        .update = P_UpdatePlayer,
        .contact = P_PlayerContact
    },
    {   // TYPE_TREE
        .glyph = { CHAR_CLUB, GREEN, TRANSP },
        .flags = OF_SOLID,
        .maxhealth = 0,
        .name = "Tree",
        .hud = "",
        .update = NULL,
        .contact = NULL,
    },
    
    {   // TYPE_ROCK1,
        .glyph = { 177, GRAY, BLACK },
        .flags = OF_SOLID,
        .maxhealth = 0,
        .name = "Rock 1",
        .hud = "",
        .update = NULL,
        .contact = NULL,
    },
    {   // TYPE_ROCK2,
        .glyph = { 177, WHITE, BLACK },
        .flags = OF_SOLID,
        .maxhealth = 0,
        .name = "Rock 2",
        .hud = "",
        .update = NULL,
        .contact = NULL,
    },
    {   // TYPE_ROCK3,
        .glyph = { 178, GRAY, BLACK },
        .flags = OF_SOLID,
        .maxhealth = 0,
        .name = "Rock 3",
        .hud = "",
        .update = NULL,
        .contact = NULL,
    },
    {   // TYPE_ROCK4,
        .glyph = { 178, WHITE, BLACK },
        .flags = OF_SOLID,
        .maxhealth = 0,
        .name = "Rock 4",
        .hud = "",
        .update = NULL,
        .contact = NULL,
    },

    {   // TYPE_WATER
        .glyph = { CHAR_NUL, BRIGHTBLUE, BLUE },
        .flags = OF_SOLID,
        .maxhealth = 0,
        .name = "Water",
        .hud = "",
        .update = A_UpdateWater,
        .contact = NULL
    },
    
    {   // TYPE_GRASS1
        .glyph = { 176, GREEN, TRANSP },
        .flags = 0,
        .maxhealth = 0,
        .name = "Grass 1",
        .hud = "",
        .update = NULL,
        .contact = NULL
    },
    {   // TYPE_GRASS2
        .glyph = { 176, BRIGHTGREEN, TRANSP },
        .flags = 0,
        .maxhealth = 0,
        .name = "Grass 2",
        .hud = "",
        .update = NULL,
        .contact = NULL
    },
    {   // TYPE_GRASS3
        .glyph = { 177, GREEN, TRANSP },
        .flags = 0,
        .maxhealth = 0,
        .name = "Grass 3",
        .hud = "",
        .update = NULL,
        .contact = NULL
    },
    {   // TYPE_GRASS4
        .glyph = { 177, BRIGHTGREEN, TRANSP },
        .flags = 0,
        .maxhealth = 0,
        .name = "Grass 4",
        .hud = "",
        .update = NULL,
        .contact = NULL
    },
    {   // TYPE_STONE1
        .glyph = { 254, GRAY, BLACK },
        .flags = OF_SOLID,
        .maxhealth = 0,
        .name = "Stone Wall 1",
        .hud = "",
        .update = NULL,
        .contact = NULL
    },
    {   // TYPE_STONE2
        .glyph = { 254, WHITE, GRAY },
        .flags = OF_SOLID,
        .maxhealth = 0,
        .name = "Stone Wall 2",
        .hud = "",
        .update = NULL,
        .contact = NULL
    },
    {   // TYPE_DIRT
        .glyph = { 177, BROWN, GRAY },
        .flags = OF_SOLID|OF_BREAKABLE,
        .maxhealth = 1,
        .name = "Dirt",
        .hud = "",
        .update = NULL,
        .contact = NULL
    },


    
    {   // TYPE_SPIDER
        .glyph = { '*', GRAY, TRANSP },
        .flags = OF_ENTITY|OF_SOLID|OF_DAMAGING,
        .maxhealth = 1,
        .name = "Spider",
        .hud = "You were devoured by a giant spider",
        .update = A_UpdateSpider,
        .contact = A_SpiderContact
    },
        
    {   // TYPE_NESSIE
        .glyph = { 244, BRIGHTGREEN, TRANSP },
        .flags = OF_ENTITY,
        .maxhealth = 40,
        .name = "Nessie",
        .hud = "",
        .update = A_NessieUpdate,
        .contact = NULL
    },
    {   // TYPE_ORGE
        .glyph = { 148, BROWN, TRANSP },
        .flags = OF_ENTITY|OF_SOLID|OF_DAMAGING,
        .maxhealth = 3,
        .name = "Orge",
        .hud = "You were thwumped by an ogre!",
        .update = A_OgreUpdate,
        .contact = NULL
    },
    {   // TYPE_CORPSE
        .glyph = { '@', RED, TRANSP },
        .flags = OF_ENTITY,
        .maxhealth = 0,
        .name = "Corpse",
        .hud = "",
        .update = NULL,
        .contact = NULL
    },

    
    {   // TYPE_GOLDKEY
        .glyph = { 229, YELLOW, TRANSP },
        .flags = OF_ENTITY,
        .maxhealth = 0,
        .name = "Gold Key",
        .hud = "Picked up a Gold Key",
        .update = NULL,
        .contact = P_CollectItem
    },
    {   // TYPE_BLUEKEY
        .glyph = { 229, BLUE, TRANSP },
        .flags = OF_ENTITY,
        .maxhealth = 0,
        .name = "Blue Key",
        .hud = "Picked up the Blue Key",
        .update = NULL,
        .contact = P_CollectItem
    },
    {   // TYPE_GREENKEY
        .glyph = { 229, BRIGHTGREEN, TRANSP },
        .flags = OF_ENTITY,
        .maxhealth = 0,
        .name = "Green Key",
        .hud = "Picked up the Green Key",
        .update = NULL,
        .contact = P_CollectItem
    },
    {   // TYPE_GOLDDOOR
        .glyph = { 8, GRAY, YELLOW },
        .flags = OF_SOLID,
        .maxhealth = 0,
        .name = "Gold Door",
        .hud = "You need a gold key to open this door!",
        .update = NULL,
        .contact = NULL
    },
    {   // TYPE_BLUEDOOR
        .glyph = { 8, GRAY, BLUE },
        .flags = OF_SOLID,
        .maxhealth = 0,
        .name = "Blue Door",
        .hud = "You need a blue key to open this door!",
        .update = NULL,
        .contact = NULL
    },
    {   // TYPE_GOLDDOOR
        .glyph = { 8, GRAY, BRIGHTGREEN },
        .flags = OF_SOLID,
        .maxhealth = 0,
        .name = "Green Door",
        .hud = "You need a green key to open this door!",
        .update = NULL,
        .contact = NULL
    },


    {   // TYPE_SWITCH
        .glyph = { 254, BROWN, TRANSP },
        .flags = 0,
        .maxhealth = 0,
        .name = "Button",
        .hud = "",
        .update = NULL,
        .contact = NULL
    },
    {   // TYPE_BLOCK
        .glyph = { 254, BLACK, BROWN },
        .flags = OF_PUSHABLE,
        .maxhealth = 0,
        .name = "Bryte-Block",
        .hud = "",
        .update = NULL,
        .contact = NULL,
    },
    
    {   // TYPE_HEART
        .glyph = { CHAR_HEART, RED, TRANSP },
        .flags = OF_ENTITY,
        .maxhealth = 0,
        .name = "Heart",
        .hud = "You got a health heart!",
        .update = NULL,
        .contact = P_CollectItem
    },
    
    {   // TYPE_RAFT
        .glyph = { 240, GRAY, BROWN },
        .flags = OF_ENTITY|OF_ITEM,
        .maxhealth = 0,
        .name = "Raft",
        .hud = "You got a raft!",
        .update = NULL,
        .contact = P_CollectItem
    },



    {   // TYPE_EXIT
        .glyph = { 239, MAGENTA|BLINK, TRANSP },
        .flags = 0,
        .maxhealth = 0,
        .name = "Exit",
        .hud = "",
        .update = NULL,
        .contact = NULL,
    },



    {   // NUMTYPES (placeholder)
        .glyph = { CHAR_NUL, TRANSP, TRANSP }
    },
    
    //==========================================================================

    {   // TYPE_PROJ_BALL
        .glyph = { CHAR_DOT1, YELLOW, TRANSP },
        .flags = OF_ENTITY|OF_DAMAGING,
        .maxhealth = 0,
        .name = "Ball Projectile",
        .update = A_UpdateProjectile,
        .contact = A_ProjectileContact
    },
    
    {   // TYPE_PROJ_RING
        .glyph = { 9, MAGENTA, TRANSP },
        .flags = OF_ENTITY|OF_DAMAGING,
        .maxhealth = 0,
        .name = "Ring Projectile",
        .hud = "You were blasted by a death ring!",
        .update = A_UpdateProjectile,
        .contact = A_ProjectileContact
    },
};



const char *ObjName (obj_t *obj)
{
    if (obj)
        return objdefs[obj->type].name;
    else
        return NULL;
}


objtype_t ObjectAtXY (tile x, tile y)
{
    return map.foreground[y][x].type;
}


const char *ObjectNameAtXY (tile x, tile y)
{
    return objdefs[ObjectAtXY(x, y)].name;
}


glyph_t *ObjectGlyphAtXY (tile x, tile y)
{
    return &map.foreground[y][x].glyph;
}


bool ObjectsOverlap (obj_t *obj1, obj_t *obj2)
{
    return obj1->x == obj2->x && obj1->y == obj2->y;
}



int RunTimer (obj_t *obj)
{
    if (obj->tics)
        obj->tics--;
    return obj->tics;
}



bool TryMove (obj_t *obj, tile x, tile y)
{
    obj_t *check;
    
    // keep object within map
    if ( x < 0 || x >= MAP_W || y < 0 || y >= MAP_H )
        return false;

    if ( (map.foreground[y][x].flags & OF_SOLID) )
    {
        return false;
    }
    
    // don't walk over solid entities
    if ( obj->flags & OF_ENTITY )
    {
        check = objlist;
        do {
            if ( (check->flags & OF_SOLID) && check->x == x && check->y == y )
            {
                if (obj->contact) 
                    obj->contact(obj, check);
                return false;
            }
            check = check->next;
        } while (check);
    }

    obj->x = x;
    obj->y = y;

    return true;
}


void DamageObj (obj_t *obj, int damage)
{
    if (obj->type == TYPE_PLAYER && obj->hittimer)
        return;
    obj->hittimer = 60;
    obj->hp -= damage;
}


void RemoveObj (obj_t *obj)
{
    ChangeObject(obj, TYPE_NONE, 0);
}



void FlashObject (obj_t *obj, int *timer, int color)
{
    if (*timer)
    {
        (*timer)--;
        if (*timer % 2)
            obj->glyph.fg_color = color;
        else
            obj->glyph.fg_color = obj->info->glyph.fg_color;
    }
}



#pragma mark - Object List


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
    printf("removed type \"%s\"\n", ObjName(rem));
    
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


int List_Count (void)
{
    obj_t *obj;
    int count;
    
    count = 0;
    obj = objlist;
    while (obj) {
        count++;
        obj = obj->next;
    }
    return count;
}



#define draw_x(n)   (n * TILE_SIZE + maprect.x)
#define draw_y(n)   (n * TILE_SIZE + maprect.y)

// draw all entity objects except player
void List_DrawObjects (void)
{
    obj_t *obj;

    if (!objlist)
        return;
    
    obj = objlist;
    do {
        if (obj->type != TYPE_NONE && obj->type != TYPE_PLAYER)
            DrawGlyph(&obj->glyph, draw_x(obj->x), draw_y(obj->y), PITCHBLACK);
        obj = obj->next;
    } while (obj);
}



objtype_t List_ObjectAtXY (tile x, tile y)
{
    obj_t *obj;
    
    obj = objlist;
    do
    {
        if (obj->x == x && obj->y == y)
            return obj->type;
        obj = obj->next;
    } while (obj);
    
    return TYPE_NONE;
}



#pragma mark -



void DrawObject (obj_t *obj)
{
    int winx, winy;
    
    if (obj->type == TYPE_NONE)
        return; // don't bother
    
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
obj_t NewObjectFromDef (objtype_t type, tile x, tile y)
{
    obj_t new;
    objdef_t *info;
    
    memset(&new, 0, sizeof(new));
    
    new.type = type;
    new.x = x;
    new.y = y;
    
    // initial default definition values
    info = &objdefs[type];
    new.info = info;
    new.glyph = info->glyph;
#if 0
    if (type == TYPE_STONE2 && (x+y) % 2 == 0)
    {
        int temp = new.glyph.fg_color;
        new.glyph.fg_color = new.glyph.bg_color;
        new.glyph.bg_color = temp;
    }
#endif
    new.hp = info->maxhealth;
    new.flags = info->flags;
    new.update = info->update;
    new.contact = info->contact;
    
    return new;
}


void ChangeObject (obj_t *obj, objtype_t type, int state)
{
    obj_t *next;
    
    printf("changing obj of type %s to type %s...\n", ObjName(obj), objdefs[type].name);
    
    next = obj->next; // save it because NewObject resets it
    *obj = NewObjectFromDef(type, obj->x, obj->y);
    obj->next = next;
    obj->state = state;
}
