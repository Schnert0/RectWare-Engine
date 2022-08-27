#include "entCamera.h"

static const char* type = "entCamera";

bool EntCamera_New(const char* name) {
  EntCamera_t* this = malloc(sizeof(EntCamera_t));
  if (!this)
    return false;
  memset(this, 0, sizeof(EntCamera_t));

  this->zoom = 32.0f;

  if (EMS_CreateEntity(name, type, (FuncDelete_t)EntCamera_Delete, (FuncUpdate_t)EntCamera_Update, (FuncDraw_t)EntCamera_Draw, this, 0))
    return true;

  free(this);

  return false;
}


void EntCamera_Delete(struct EntCamera_t* this) {
  if (this)
    free(this);
}


void EntCamera_Update(struct EntCamera_t* this) {

}


void EntCamera_Draw(struct EntCamera_t* this) {

}
