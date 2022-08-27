#ifndef ENTTILELAYER_H
#define ENTTILELAYER_H

#include "../../common.h"
#include "../../engine/engine.h"
#include "../../engine/core/ems.h"

#include "entCamera.h"

typedef struct EntTileLayer_t {
  int32_t  width, height;
  int32_t  textureID;
  uint8_t* tiles;
  vec2f_t  parallaxFactor;
} EntTileLayer_t;


bool EntTileLayer_New(const char* name);
void EntTileLayer_Delete(struct EntTileLayer_t* this);
void EntTileLayer_Update(struct EntTileLayer_t* this);
void EntTileLayer_Draw(struct EntTileLayer_t* this);

uint8_t EntTileLayer_GetTile(struct EntTileLayer_t* this, float x, float y);

#endif // ENTTILELAYER_H
