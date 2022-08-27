#include "entPlayer.h"

static const char* type = "entPlayer";
struct EntPlayer_t;

enum {
  PLAYERSTATE_IDLE,
  PLAYERSTATE_SPEEDUP,
  PLAYERSTATE_RUN,
  PLAYERSTATE_SLOWDOWN,

  PLAYERSTATE_JUMPING,
  PLAYERSTATE_FALLING,

  MAX_PLAYERSTATES
};


const float PLAYER_MINSPEED = 0.125f;

const float PLAYER_FRICTION_GROUND = 0.25f;
const float PLAYER_FRICTION_AIR    = 0.125f;

bool EntPlayer_New(const char* name) {
  EntPlayer_t* this = malloc(sizeof(EntPlayer_t));
  if (!this)
    return false;
  memset(this, 0, sizeof(EntPlayer_t));


  this->pos.x = 0.0f;
  this->pos.y = 0.0f;
  this->state = PLAYERSTATE_IDLE;

  this->drawDimensions.x = 64;
  this->drawDimensions.y = 64;
  this->textureID = Renderer_LoadTexture("assets/Player_idle.bmp");

  if (EMS_CreateEntity(name, type, (FuncDelete_t)EntPlayer_Delete, (FuncUpdate_t)EntPlayer_Update, (FuncDraw_t)EntPlayer_Draw, this, 0))
    return true;

  free(this);

  return false;
}


void EntPlayer_Delete(struct EntPlayer_t* this) {
  if (this)
    free(this);

}


void EntPlayer_Update(struct EntPlayer_t* this) {
  this->pos.x += this->vel.x;
  this->pos.y += this->vel.y;

  switch (this->state) {
  case PLAYERSTATE_IDLE: EntPlayer_UpdateIdle(this); break;

  default:
    EntPlayer_UpdateIdle(this);
    this->state = PLAYERSTATE_IDLE;
  }
}


void EntPlayer_Draw(struct EntPlayer_t* this) {
  int32_t winW, winH;
  Renderer_GetWindowSize(&winW, &winH);
  
  rect_t dst = {
    this->pos.x - (this->drawDimensions.x * 0.5f) + (winW * 0.5f),
    this->pos.y - (this->drawDimensions.y * 0.5f) + (winH * 0.5f),
    this->drawDimensions.x,
    this->drawDimensions.y
  };

  Renderer_PushTexturedRect(this->textureID, &dst, NULL);
}


void EntPlayer_UpdateIdle(struct EntPlayer_t* this) {

}


void EntPlayer_UpdateSlowdown(struct EntPlayer_t* this) {
  if (this->vel.x > PLAYER_MINSPEED) {
    this->vel.x -= this->friction;
  } else if (this->vel.x < -PLAYER_MINSPEED) {
    this->vel.x += this->friction;
  } else {
    this->vel.x = 0.0f;
    this->state = PLAYERSTATE_IDLE;
  }
}
