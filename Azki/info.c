//
//  info.c
//  Azki
//
//  Created by Thomas Foster on 5/8/20.
//  Copyright © 2020 Thomas Foster. All rights reserved.
//

#include "obj.h"
#include "video.h"
#include "glyph.h"

void A_UpdateWater (obj_t *water);
void A_ExitContact (obj_t *exit, obj_t *hit);
void A_Flicker (obj_t *obj);

void A_SpiderUpdate (obj_t *sp);
void A_SpiderContact (obj_t *sp, obj_t *hit);
void A_NessieUpdate (obj_t *n);
void A_OgreUpdate (obj_t *ogre);
void A_OgreContact (obj_t *ogre, obj_t *hit);

void A_SpawnProjectile (objtype_t type, obj_t *src, obj_t *dst, int dx, int dy, int delay, int damage);
void A_UpdateProjectile (obj_t *b);
void A_ProjectileContact (obj_t *b, obj_t *hit);

void P_UpdatePlayer (obj_t * pl);
void P_PlayerContact (obj_t *pl, obj_t *hit);
void P_CollectItem (obj_t *item, obj_t *entity);


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
        .flags = OF_ENTITY,
        .maxhealth = 4,
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
        .flags = OF_ENTITY|OF_DAMAGING|OF_SOLID,
        .maxhealth = 1,
        .name = "Spider",
        .hud = "You were devoured by a giant spider!",
        .update = A_SpiderUpdate,
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
        .flags = OF_ENTITY|OF_DAMAGING|OF_SOLID,
        .maxhealth = 3,
        .name = "Orge",
        .hud = "You were thwumped by an ogre!",
        .update = A_OgreUpdate,
        .contact = A_OgreContact
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
    {   // TYPE_GREENDOOR
        .glyph = { 8, GRAY, BRIGHTGREEN },
        .flags = OF_SOLID,
        .maxhealth = 0,
        .name = "Green Door",
        .hud = "You need a green key to open this door!",
        .update = NULL,
        .contact = NULL
    },
    {   // TYPE_DOOR
        .glyph = { 8, GRAY, BRIGHTWHITE },
        .flags = OF_SOLID,
        .maxhealth = 0,
        .name = "Door",
        .hud = "",
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
        .flags = OF_PUSHABLE|OF_ENTITY|OF_SOLID,
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
    {// TYPE_CANDLE
        .glyph = { 161, YELLOW, TRANSP },
        .flags = OF_SOLID,
        .maxhealth = 0,
        .name = "Candle",
        .hud = "",
        .update = A_Flicker,
        .contact = NULL
    },
 
    {   // TYPE_BAZOOKA
        .glyph = { 12, MAGENTA, TRANSP },
        .flags = OF_ITEM|OF_ENTITY,
        .maxhealth = 0,
        .name = "Arcane Bazooka",
        .hud = "You got the Arcane Bazooka!",
        .update = NULL,
        .contact = P_CollectItem
    },

    {   // TYPE_EXIT
        .glyph = { 239, MAGENTA|BLINK, TRANSP },
        .flags = OF_ENTITY,
        .maxhealth = 0,
        .name = "Exit",
        .hud = "",
        .update = NULL,
        .contact = A_ExitContact
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
