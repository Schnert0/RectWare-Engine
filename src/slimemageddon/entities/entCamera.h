#ifndef ENTCAMERA_H
#define ENTCAMERA_H

#include "../../common.h"
#include "../../engine/engine.h"
#include "../../engine/core/ems.h"


typedef struct EntCamera_t {
  vec2f_t pos;
  float   zoom;
} EntCamera_t;


bool EntCamera_New(const char* name);
void EntCamera_Delete(struct EntCamera_t* this);
void EntCamera_Update(struct EntCamera_t* this);
void EntCamera_Draw(struct EntCamera_t* this);

#endif // ENTCAMERA_H
