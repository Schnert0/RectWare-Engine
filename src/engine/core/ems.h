#ifndef EMS_H
#define EMS_H

#include "../../common.h"
#include "../engine.h"

#define MAX_ENTITIES 2048

typedef void (*FuncUpdate_t)(void* data);
typedef void (*FuncDraw_t)(void* data);
typedef void (*FuncDelete_t)(void* data);

typedef struct {
  union {
    char* name; // Name of the entity
    char* key;  // stb_ds requires a field named "key"
  };
  char*         type;     // Type of the entity
  FuncDelete_t  delete;   // Function pointer to entity delete function
  FuncUpdate_t  update;   // Function pointer to entity update function
  FuncDraw_t    draw;     // Function pointer to entity draw function
  void*         userData; // Function pointer to entity data
  int32_t       depth;    // Sorting priority of entity
  bool          isActive; // Is this entity slot open for use?
} EntitySlot_t;

typedef struct {
  EntitySlot_t*  entities;
  bool          shouldSort;
} EMS_t;


bool EMS_Init();
void EMS_Cleanup();

void EMS_Update();
void EMS_Draw();

bool EMS_CreateEntity(const char* name, const char* type, FuncDelete_t delete, FuncUpdate_t update, FuncDraw_t draw, void* userData, int32_t depth);
void EMS_DeleteEntity(const char* name);
void EMS_ClearEntities();

void* EMS_GetEntityByName(const char* name);

#endif // EMS_H
