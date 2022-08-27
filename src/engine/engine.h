#ifndef ENGINE_H
#define ENGINE_H

#include "../common.h"

#include "core/renderer.h"
#include "core/input.h"
#include "core/ems.h"

typedef struct {
  bool temp;
} Engine_t;

bool Engine_Init();
void Engine_Cleanup();

void Engine_Run();

// Logging functions
void Engine_Fatal(const char* message, ...);
void Engine_Error(const char* message, ...);
void Engine_Warning(const char* message, ...);
void Engine_Log(const char* message, ...);

#endif // ENGINE_H
