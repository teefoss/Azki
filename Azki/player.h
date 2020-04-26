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

extern obj_t *player;

void P_FireBullet (dir_t dir);
void P_UpdatePlayer (obj_t * pl);
void P_PlayerContact (obj_t *pl, obj_t *hit);

#endif /* player_h */
