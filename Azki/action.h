//
//  action.h
//  Azki
//
//  Created by Thomas Foster on 4/20/20.
//  Copyright © 2020 Thomas Foster. All rights reserved.
//

#ifndef action_h
#define action_h

#include "obj.h"

void A_UpdateWater (obj_t *water);
void A_UpdateSpider (obj_t *sp);

void A_FireBulletDir (obj_t *src, dir_t dir, int damage);
void A_UpdateBullet (obj_t *b);
void A_BulletContact (obj_t *b, obj_t *hit);

void A_NessieUpdate (obj_t *n);

#endif /* action_h */
