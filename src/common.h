#ifndef COMMON_H
#define COMMON_H

// Standard libraries
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Graphics libraries
#include <SDL2/SDL.h>
#include <GL/glew.h>

// Internal libraries
#include "lib/stb_ds.h"
#include "lib/stb_image.h"
#include "lib/stb_rect_pack.h"

// Universal data types 
typedef struct { float   x, y;       } vec2f_t;
typedef struct { float   x, y, z;    } vec3f_t;
typedef struct { float   x, y, z, w; } vec4f_t;
typedef struct { float   x, y, w, h; } rect_t;
typedef struct { uint8_t r, g, b, a; } color_t;

#endif // COMMON_H
