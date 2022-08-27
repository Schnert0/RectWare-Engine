#include "entTileLayer.h"

static const char* type = "entTileLayer";

struct EntCamera_t;

static const uint8_t tiles[] = {
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1
};

bool EntTileLayer_New(const char* name) {
  EntTileLayer_t* this = malloc(sizeof(EntTileLayer_t));
  if (!this)
    return false;
  memset(this, 0, sizeof(EntTileLayer_t));

  this->textureID = Renderer_LoadTexture("assets/slimemageddon/tilesets/House.png");

  this->width = 8;
  this->height = 8;
  this->tiles = (uint8_t*)tiles;

  if (EMS_CreateEntity(name, type, (FuncDelete_t)EntTileLayer_Delete, (FuncUpdate_t)EntTileLayer_Update, (FuncDraw_t)EntTileLayer_Draw, this, 0))
    return true;

  free(this);

  return false;
}


void EntTileLayer_Delete(struct EntTileLayer_t* this) {
  if (this)
    free(this);
}


void EntTileLayer_Update(struct EntTileLayer_t* this) {
}


void EntTileLayer_Draw(struct EntTileLayer_t* this) {
  EntCamera_t* camera = EMS_GetEntityByName("camera");

  if (camera->zoom < 1.0f)
    return;

  vec2f_t parallax = this->parallaxFactor;
  vec2f_t pos = camera->pos;
  float   zoom = camera->zoom;

  vec2f_t center = { pos.x * parallax.x, pos.y * parallax.y };
  vec2f_t offset = { center.x - (int32_t)center.x, center.y - (int32_t)center.y };

  int32_t winW, winH;
  Renderer_GetWindowSize(&winW, &winH);

  int32_t halfW = ((winW / zoom) * 0.5f) + 2;
  int32_t halfH = ((winH / zoom) * 0.5f) + 2;

  float halfWinW = winW * 0.5f;
  float halfWinH = winH * 0.5f;

  for (int32_t y = -halfH; y < halfH; y++) {
    for (int32_t x = -halfW; x < halfW; x++) {
      uint8_t tile = EntTileLayer_GetTile(this, center.x + x, center.y + y);
      if (tile) {
        rect_t src = { (tile & 0x0f) << 5, (tile & 0xf0) << 1, 32, 32 };
        rect_t dst = { x-offset.x, y-offset.y, zoom, zoom };
        dst.x *= zoom;
        dst.y *= zoom;
        dst.x += halfWinW;
        dst.y += halfWinH;
        Renderer_PushTexturedRect(this->textureID, &dst, &src);
      }
    }
  }
}


uint8_t EntTileLayer_GetTile(struct EntTileLayer_t* this, float x, float y) {
  if (this->tiles && x >= 0 && x < this->width && y >= 0 && y < this->height)
    return this->tiles[(int32_t)x + ((int32_t)y * this->width)];

  return 0x00;
}

// Renderer_t* renderer = tileLayer->base.game->renderer;
// Camera_t* camera = tileLayer->base.game->camera;
//
// if (camera->zoom < 1.0)
//   return;
//
// Vec2f_t parallax = tileLayer->parallax;
// Vec2f_t pos = camera->pos;
// float zoom = camera->zoom;
//
// float centerX = pos.x*parallax.x;
// float centerY = pos.y*parallax.y;
//
// float offsetX = centerX - (int32_t)centerX;
// float offsetY = centerY - (int32_t)centerY;
//
// int32_t halfW = ((renderer->windowWidth  / zoom) / 2) + 2;
// int32_t halfH = ((renderer->windowHeight / zoom) / 2) + 2;
//
// for (int32_t y = -halfH; y < halfH; y++) {
//   for (int32_t x = -halfW; x < halfW; x++) {
//     uint8_t tile = EntTileLayer_GetTile(tileLayer, centerX+x, centerY+y);
//     if (tile) {
//       float drawX = x-offsetX;
//       float drawY = y-offsetY;
//       drawX *= zoom;
//       drawY *= zoom;
//       drawX += renderer->windowWidth/2;
//       drawY += renderer->windowHeight/2;
//       drawX = floor(drawX+0.5f);
//       drawY = floor(drawY+0.5f);
//       Renderer_DrawTexture(renderer, false, tileLayer->parallax, tileLayer->tileset, (tile&0x0f) << 5, (tile&0xf0) << 1, tileLayer->tileW, tileLayer->tileH, drawX, drawY, ceil(zoom+0.5f), ceil(zoom+0.5f));
//     }
//   }
// }
