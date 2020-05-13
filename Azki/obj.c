//
//  obj.c
//  Azki
//
//  Created by Thomas Foster on 4/1/20.
//  Copyright © 2020 Thomas Foster. All rights reserved.
//

#include <string.h>
#include <math.h>
#include "obj.h"
#include "player.h"
#include "video.h"
#include "map.h"

// singly linked list of active (mobile) entities
obj_t *objlist;


const char *ObjName (obj_t *obj)
{
    if (obj)
        return objdefs[obj->type].name;
    else
        return NULL;
}

objtype_t ObjectTypeAtXY (tile x, tile y)
{
    return map.foreground[y][x].type;
}


const char *ObjectNameAtXY (tile x, tile y)
{
    return objdefs[ObjectTypeAtXY(x, y)].name;
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
        
    // don't walk over solid entities, contact
#if 1
    if ( obj->flags & OF_ENTITY )
    {
        check = objlist;
        do {
            if ( (check->flags & OF_SOLID) && check->x == x && check->y == y )
            {
//                if (obj->contact)
//                    obj->contact(obj, check);
                return false;
            }
            check = check->next;
        } while (check);
    }
#endif

    obj->x = x;
    obj->y = y;

    return true;
}



int ObjectDistance (obj_t *obj1, obj_t *obj2)
{
    int dx, dy;
    
    dx = obj2->x - obj1->x;
    dy = obj2->y - obj1->y;
    return (int)sqrt(dx*dx + dy*dy);
}


void DamageObj (obj_t *src, obj_t *target, int damage)
{
    switch (target->type)
    {
        case TYPE_PLAYER:
            if (target->hittimer)
                return;
            target->hittimer = 60;
            break;
            
        default:
            target->hittimer = 30;
            break;
    }
    
    target->hp -= damage;
    if (target->type == TYPE_PLAYER) {
        if (target->hp <= 0) {
            strncpy(deathmsg, src->info->hud, 80);
        }
    }
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
    
    new = malloc(sizeof(obj_t));
    if (!new)
        Quit("List_AddObject: error, could not alloc mem");
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
        Quit("List_RemoveObject: error, tried to remove NULL object!");
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
