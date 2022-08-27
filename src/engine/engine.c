#include "engine.h"

static Engine_t this;

// Initialize the Engine and components
bool Engine_Init() {
  // Engine object
  memset(&this, 0, sizeof(Engine_t));

  // Components
  if (!Renderer_Init()) return false;
  if (!Input_Init())    return false;
  if (!EMS_Init())      return false;

  return true;
}


// Cleanup Engine and components
void Engine_Cleanup() {
  EMS_Cleanup();
  Input_Cleanup();
  Renderer_Cleanup();
}

#include "entities/entCamera.h"
#include "entities/entPlayer.h"
#include "entities/entTileLayer.h"

// Run main game loop
void Engine_Run() {
  EntCamera_New("camera");
  EntPlayer_New("player");
  EntTileLayer_New("tileLayer");
  
  while (Input_GetInput()) {
    Renderer_PushRect(NULL, (color_t){ 0x00, 0x00, 0x00, 0xff });
    EMS_Update();
    EMS_Draw();
    Renderer_Update();
  }
}


// Logs fatal error message to the console and shuts down the engine
void Engine_Fatal(const char* message, ...) {
  if(message == NULL){
    printf("\x1b[31m[FATAL]\x1b[0m unknown fatal error with NULL pointer thrown\n");
    Engine_Cleanup();
    exit(0);
  }

  char buffer[1024];
  va_list args;
  va_start(args, message);
  vsnprintf(buffer, sizeof(buffer), message, args);
  va_end(args);

  printf("\x1b[31m[FATAL]\x1b[0m %s\n", buffer);

  Renderer_DisplayError(buffer);

  Engine_Cleanup();
  exit(0);
}


// Logs error message to the console
void Engine_Error(const char* message, ...) {
  if(message == NULL){
    printf("\x1b[31m[ERROR]\x1b[0m unknown error with NULL pointer thrown\n");
    return;
  }

  char buffer[1024];
  va_list args;
  va_start(args, message);
  vsnprintf(buffer, sizeof(buffer), message, args);
  va_end(args);

  printf("\x1b[31m[ERROR]\x1b[0m %s\n", buffer);
}


// Logs warning message to the console
void Engine_Warning(const char* message, ...) {
  if(message == NULL)
    return;

  char buffer[1024];
  va_list args;
  va_start(args, message);
  vsnprintf(buffer, sizeof(buffer), message, args);
  va_end(args);

  printf("\x1b[33m[WARNING]\x1b[0m %s\n", buffer);
}


// Logs message to the console
void Engine_Log(const char* message, ...) {
  if(message == NULL)
    return;

  char buffer[1024];
  va_list args;
  va_start(args, message);
  vsnprintf(buffer, sizeof(buffer), message, args);
  va_end(args);

  printf("[LOG] %s\n", buffer);
}
