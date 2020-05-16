//
//  player.h
//  Azki
//
//  Created by Thomas Foster on 4/3/20.
//  Copyright © 2020 Thomas Foster. All rights reserved.
//

#ifndef player_h
#define player_h

#include "obj.h"

typedef struct
{
    bool goldkey;
    bool bluekey;
    bool greenkey;
    bool boat;
} inventory_t;

typedef enum
{
    WEAPON_SWORD,
    WEAPON_BAZOOKA,
    WEAPON_COUNT
} weapontype_t;

typedef struct
{
    obj_t           *obj; // linked to player obj in objlist at start of level
    inventory_t     items;
    int             maxhealth;
    bool            weapons[WEAPON_COUNT];
    weapontype_t    current_weapon;
    
    // timers
    int             movedelay;
    int             shotdelay;
    int             cooldown; // enemy hit
    int             itempickup;
    
} player_t;

extern player_t player;
extern dir_t sword_dir;

void InitPlayer (void);
void P_PlayerInput (void);
void P_SwitchWeapon (weapontype_t w);
void P_DrawInventory (void);
void P_DrawHealth (void);
void P_DrawSword (void);
void P_DrawPlayer (void);

#endif /* player_h */
