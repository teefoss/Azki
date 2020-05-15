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
} playeritems_t;

typedef enum
{
    WEAPON_SWORD,
    WEAPON_BAZOOKA,
    WEAPON_COUNT
} weapontype_t;

extern obj_t *player;
extern playeritems_t items;
extern dir_t sword_dir;
extern weapontype_t current_weapon;

void InitPlayer (void);
void P_PlayerInput (void);
void P_SwitchWeapon (weapontype_t w);
void P_FireBullet (dir_t dir);
void P_DrawInventory (void);
void P_DrawHealth (void);
void P_DrawSword (void);
void P_DrawPlayer (void);

#endif /* player_h */
