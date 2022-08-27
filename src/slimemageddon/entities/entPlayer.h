#ifndef ENTPLAYER_H
#define ENTPLAYER_H

#include "../../common.h"
#include "../../engine/engine.h"
#include "../../engine/core/ems.h"

#include "entCamera.h"

typedef struct EntPlayer_t {
  vec2f_t  pos, vel;
  float    friction;
  uint16_t state;

  vec2f_t drawDimensions;
  int32_t textureID;
} EntPlayer_t;


bool EntPlayer_New(const char* name);
void EntPlayer_Delete(struct EntPlayer_t* this);
void EntPlayer_Update(struct EntPlayer_t* this);
void EntPlayer_Draw(struct EntPlayer_t* this);

void EntPlayer_UpdateIdle(struct EntPlayer_t* this);

#endif // ENTPLAYER_H
