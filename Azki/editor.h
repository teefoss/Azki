//
//  editor.h
//  Azki
//
//  Created by Thomas Foster on 4/2/20.
//  Copyright © 2020 Thomas Foster. All rights reserved.
//

#ifndef editor_h
#define editor_h

#include "map.h"

typedef enum {
    LAYER_FG,
    LAYER_BG,
    LAYER_BOTH
} layer_t;

extern layer_t showlayer;

void EditorLoop (void);

#endif /* editor_h */
