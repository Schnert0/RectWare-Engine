#ifndef RENDERER_H
#define RENDERER_H

#include "../../common.h"
#include "../engine.h"

#define MAX_TEXTURES 4096
#define MAX_VERTICES 1024*1024
#define MAX_INDICES 1024*1024*4
#define MAX_ATLAS_DIMENSIONS 4096
#define MAX_RECT_PACK_NODES 4096

enum {
  TEXTURESTATE_EMPTY,
  TEXTURESTATE_UNUSED,
  TEXTURESTATE_USED
};

typedef struct {
  uint32_t state;
  rect_t  rect;
} Texture_t;

typedef struct {
  uint16_t x, y;
  uint16_t u, v;
  color_t color;
} Vert_t;

typedef struct {
  SDL_Window* window;
  SDL_GLContext* ctx;

  GLuint shaderProgram;
  GLuint vbo, vao, ebo;
  GLuint atlasID;

  int32_t windowWidth, windowHeight;
  // float invHalfWidth, invHalfHeight;
  bool isWireframe;

  Vert_t* vertMap;
  uint16_t* indexMap;

  stbrp_context rectPackCtx;
  stbrp_node* rectPackNodes;

  Texture_t textures[MAX_TEXTURES];

  uint32_t currVert;
  Vert_t vertices[MAX_VERTICES];

  uint32_t currIndex;
  uint32_t indices[MAX_INDICES];
} Renderer_t;

bool Renderer_Init();
void Renderer_Cleanup();

void Renderer_Update();

// Window functions
bool Renderer_CreateWindow(const char* title, int32_t width, int32_t height);
void Renderer_DisplayError(const char* message);
void Renderer_GetWindowSize(int32_t* w, int32_t* h);
void Renderer_ToggleWireframe();

// Texture functions
int32_t Renderer_LoadTexture(const char* path);

void Renderer_FreeTexture(int32_t textureID);
void Renderer_GetTexCoordsFromID(int32_t textureID, vec2f_t* minTexCoords, vec2f_t* maxTexCoords);

// Misc. drawing functions
void Renderer_PushAtlas();
void Renderer_PushClear(color_t color);

// Shape drawing functions
int32_t Renderer_PushTexturedRect(int32_t textureID, rect_t* dst, rect_t* src);
int32_t Renderer_PushRect(rect_t* rect, color_t color);
int32_t Renderer_DrawElipse(rect_t* rect, color_t color);

// Transformation
void Renderer_RotateRect(int32_t index, rect_t* rect, vec2f_t center, float angle);
void Renderer_SetVertColors(int32_t index, int32_t len, color_t* colors);

#endif // RENDERER_H
