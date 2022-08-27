#include "ems.h"

static EMS_t this;

bool EMS_Init() {
  memset(&this, 0, sizeof(EMS_t));

  sh_new_strdup(this.entities);
  if (!this.entities)
    return false;

  return true;
}


void EMS_Cleanup() {
  EMS_ClearEntities();

  if (this.entities)
    shfree(this.entities);
}


void EMS_Update() {
  int32_t numEntities = shlen(this.entities);
  int32_t entitiesLeft = numEntities;
  for (int32_t i = 0; i < numEntities; i++) {
    EntitySlot_t* slot = &this.entities[i];

    if (slot->update)
      slot->update(slot->userData);

    entitiesLeft--;
    if (entitiesLeft <= 0)
      break;
  }
}


void EMS_Draw() {
  int32_t numEntities = shlen(this.entities);
  int32_t entitiesLeft = numEntities;
  for (int32_t i = 0; i < numEntities; i++) {
    EntitySlot_t* slot = &this.entities[i];

    if (slot->draw)
      slot->draw(slot->userData);

    entitiesLeft--;
    if (entitiesLeft <= 0)
      break;
  }
}


bool EMS_CreateEntity(const char* name, const char* type, FuncDelete_t delete, FuncUpdate_t update, FuncDraw_t draw, void* userData, int32_t depth) {
  if (shlen(this.entities) >= MAX_ENTITIES) {
    Engine_Error("All entity slots are full");
    return false;
  }

  EntitySlot_t slot = { strdup(name), (char*)type, delete, update, draw, userData, depth };
  shputs(this.entities, slot);
  int32_t index = shgeti(this.entities, name);
  if (index == -1) {
    free(slot.name);
    return false;
  }

  return true;
}


static void EMS_DeleteEntityByID(int32_t index) {
  if (this.entities[index].name)
    free(this.entities[index].name);

  if (this.entities[index].delete)
    this.entities[index].delete(this.entities[index].userData);
}


void EMS_DeleteEntity(const char* name) {
  int32_t slotID = shgeti(this.entities, name);
  if (slotID == -1) {
    Engine_Error("Unable to find entity named '%s' for deletion", name);
    return;
  }

  EMS_DeleteEntityByID(slotID);
}


void EMS_ClearEntities() {
  int32_t numEntities = shlen(this.entities);
  for (int32_t i = 0; i < numEntities; i++) {
    EMS_DeleteEntityByID(i);
  }

  sh_new_strdup(this.entities);
}


void* EMS_GetEntityByName(const char* name) {
  int32_t index = shgeti(this.entities, name);
  return this.entities[index].userData;
}
