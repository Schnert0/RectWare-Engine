#include "renderer.h"

static Renderer_t this;

const char* vertexShaderSource =
  "#version 330 core\n"

  "layout (location = 0) in int attrPos;"
  "layout (location = 1) in int attrTexCoords;"
  "layout (location = 2) in int attrColor;"

  "out vec4 fColor;"
  "out vec2 fTexCoords;"

  "uniform float windowWidth;"
  "uniform float windowHeight;"
  "uniform float atlasWidth;"
  "uniform float atlasHeight;"

  "void main() {"

    "int x = ((attrPos & 0xffff) << 16) >> 16;"
    "int y = (attrPos & 0xffff0000) >> 16;"

    "gl_Position = vec4("
      "(x / 640.0f) - 1.0f,"
      "1.0f - (y / 360.0f),"
      "1.0f,"
      "1.0f"
    ");"

    "fTexCoords = vec2("
      "(((attrTexCoords      ) & 0xffff) / 4095.0f),"
      "(((attrTexCoords >> 16) & 0xffff) / 4095.0f)"
    ");"

    "fColor = vec4("
      "((attrColor      ) & 0xff) / 255.0f,"
      "((attrColor >>  8) & 0xff) / 255.0f,"
      "((attrColor >> 16) & 0xff) / 255.0f,"
      "((attrColor >> 24) & 0xff) / 255.0f"
    ");"

  "}";


const char* fragmentShaderSource =
  "#version 330 core\n"

  "in vec4 fColor;"
  "in vec2 fTexCoords;"

  "out vec4 fragColor;"

  "uniform sampler2D textureID;"

  "void main() {"
    "if (fTexCoords.x <= 1.0f && fTexCoords.y <= 1.0f) {"
      "fragColor = fColor * texture(textureID, fTexCoords);"
    "} else {"
      "fragColor = fColor;"
    "}"
  "}";

static bool _Renderer_CreateShaders();
static bool _Renderer_CheckShaderCompilation(GLuint shaderID);
static void _Renderer_SetUniformFloat(const char* name, float value);
static bool _Renderer_GenerateBuffers();
static bool _Renderer_CreateAtlas();
static void _Renderer_PushRect(int32_t textureID, rect_t* dst, rect_t* src, color_t* colors, float angle, vec2f_t offset, bool hFlip, bool vFlip);

// Initialize renderer state
bool Renderer_Init() {
  memset(&this, 0, sizeof(Renderer_t));

  // Initialize SDL for OpenGL rendering
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

  if (!Renderer_CreateWindow("RectWare Engine", 1280, 720)) return false;

  // Init GLEW
  glewExperimental = GL_TRUE;
  glewInit();

  if (!_Renderer_CreateShaders()) return false;
  if (!_Renderer_GenerateBuffers()) return false;
  if (!_Renderer_CreateAtlas()) return false;

  // Initiaize STB helper libraries
  // stbi_set_flip_vertically_on_load(true);

  this.rectPackNodes = malloc(sizeof(stbrp_node) * MAX_RECT_PACK_NODES);
  if (!this.rectPackNodes) {
    Engine_Error("Unable to create rect pack nodes");
    return false;
  }

  // stbrp_setup_heuristic(&this.rectPackCtx, STBRP__INIT_skyline);
  // stbrp_setup_allow_out_of_mem(&this.rectPackCtx, false);
  stbrp_init_target(&this.rectPackCtx, MAX_ATLAS_DIMENSIONS, MAX_ATLAS_DIMENSIONS, this.rectPackNodes, MAX_RECT_PACK_NODES);

  return true;
}


// Cleanup renderer state
void Renderer_Cleanup() {
  glUnmapBuffer(GL_ARRAY_BUFFER);
  glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

  glDeleteVertexArrays(1, &this.vao);
  glDeleteBuffers(1, &this.vbo);
  glDeleteBuffers(1, &this.ebo);
  glDeleteProgram(this.shaderProgram);

  if (this.rectPackNodes)
    free(this.rectPackNodes);

  if (this.ctx)
    SDL_GL_DeleteContext(this.ctx);

  if (this.window)
    SDL_DestroyWindow(this.window);
}


void Renderer_Update() {
  if (this.isWireframe) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  // Copy vertex and index data to GPU buffer
  memcpy(this.vertMap, this.vertices, this.currVert*sizeof(Vert_t));
  memcpy(this.indexMap, this.indices, this.currIndex*sizeof(uint32_t));

  glBindTexture(GL_TEXTURE_2D, this.atlasID);
  glBindVertexArray(this.vao);
  glDrawElements(GL_TRIANGLES, this.currIndex, GL_UNSIGNED_INT, 0);
  glBindTexture(GL_TEXTURE_2D, 0);

  SDL_GL_SwapWindow(this.window);

  this.currVert = 0;
  this.currIndex = 0;
}


// Create a window with given specifications and return if window creation was successful
bool Renderer_CreateWindow(const char* title, int32_t width, int32_t height) {
  this.windowWidth   = width;
  this.windowHeight  = height;
  // this.invHalfWidth  = 1.0f / ((float)this.windowWidth * 0.5f);
  // this.invHalfHeight = 1.0f / ((float)this.windowHeight * 0.5f);

  if (this.window)
    SDL_DestroyWindow(this.window);

  if (this.ctx)
    SDL_GL_DeleteContext(this.ctx);

  this.window = SDL_CreateWindow(title,
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    this.windowWidth, this.windowHeight,
    SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
  );
  if (!this.window)
    return false;

  this.ctx = SDL_GL_CreateContext(this.window);
  if (!this.ctx)
    return false;

  if (this.shaderProgram) {
    _Renderer_SetUniformFloat("windowWidth", this.windowWidth);
    _Renderer_SetUniformFloat("windowHeight", this.windowHeight);
  }

  return true;
}

static bool _Renderer_CreateShaders() {
  // Vertex shader
  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  // Engine_Log("Compiling vertex shader...");
  glCompileShader(vertexShader);
  if (!_Renderer_CheckShaderCompilation(vertexShader))
    return false;

  // Fragment shader
  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  // Engine_Log("Compiling fragment shader...");
  glCompileShader(fragmentShader);
  if (!_Renderer_CheckShaderCompilation(fragmentShader))
    return false;

  // Engine_Log("shaders compiled successfully.");

  // Link shaders
  this.shaderProgram = glCreateProgram();
  glAttachShader(this.shaderProgram, vertexShader);
  glAttachShader(this.shaderProgram, fragmentShader);
  glLinkProgram(this.shaderProgram);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  if (this.shaderProgram) {
    _Renderer_SetUniformFloat("windowWidth", this.windowWidth);
    _Renderer_SetUniformFloat("windowHeight", this.windowHeight);
  }

  return true;
}

// Check if shader compiled and return an error message if necessary
static bool _Renderer_CheckShaderCompilation(GLuint shaderID) {
  GLint success;
  char message[1024];

  glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
  if(!success) {
    glGetShaderInfoLog(shaderID, 1024, NULL, message);
    Engine_Error("Unable to compile shader:\n\n%s", message);

    return false;
  }

  return true;
}


static void _Renderer_SetUniformFloat(const char* name, float value) {
  GLint location = glGetUniformLocation(this.shaderProgram, name);
  glUniform1f(location, value);
}


// Generate all OpenGL buffers
static bool _Renderer_GenerateBuffers() {
  // Bind arrays
  glGenVertexArrays(1, &this.vao);
  glGenBuffers(1, &this.vbo);
  glGenBuffers(1, &this.ebo);

  glBindVertexArray(this.vao);

  glBindBuffer(GL_ARRAY_BUFFER, this.vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vert_t) * MAX_VERTICES, this.vertices, GL_STREAM_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this.ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * MAX_INDICES, this.indices, GL_STREAM_DRAW);

  // X and Y positon (is of type GL_FLOAT because of 32-bit width)
  glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(Vert_t), (void*)offsetof(Vert_t, x));
  glEnableVertexAttribArray(0);

  // U and V texture coordinates
  glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(Vert_t), (void*)offsetof(Vert_t, u));
  glEnableVertexAttribArray(1);

  // Color attributes
  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vert_t), (void*)offsetof(Vert_t, color));
  glEnableVertexAttribArray(2);

  // Enable blending
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  glUseProgram(this.shaderProgram);

  // Get vertex buffer mapping for GPU
  glBindBuffer(GL_ARRAY_BUFFER, this.vbo);
  this.vertMap = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
  if (!this.vertMap)
    return false;

  // Get index buffer mapping for GPU
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this.ebo);
  this.indexMap = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
  if (!this.indexMap)
    return false;

  return true;
}


// Display a fatal error message
void Renderer_DisplayError(const char* message) {
  if (this.window)
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "RectWare Engine - Fatal Error", message, this.window);
}


static bool _Renderer_CreateAtlas() {
  // Create blank texture to pass to GPU
  uint8_t* data = malloc(MAX_ATLAS_DIMENSIONS * MAX_ATLAS_DIMENSIONS * sizeof(uint32_t));
  if (!data) {
    Engine_Error("Unable to create blank texture for atlas");
    return false;
  }
  memset(data, 0, sizeof(uint32_t));

  glGenTextures(1, &this.atlasID);
  glBindTexture(GL_TEXTURE_2D, this.atlasID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, MAX_ATLAS_DIMENSIONS, MAX_ATLAS_DIMENSIONS, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  glBindTexture(GL_TEXTURE_2D,0);

  if (this.shaderProgram) {
    _Renderer_SetUniformFloat("atlasWidth", MAX_ATLAS_DIMENSIONS);
    _Renderer_SetUniformFloat("atlasHeight", MAX_ATLAS_DIMENSIONS);
  }

  free(data);

  return true;
}


// Display all polygons in wireframe mode
void Renderer_ToggleWireframe() {
  this.isWireframe ^= true;
  if (this.isWireframe) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}


// Get dimensions of the window
void Renderer_GetWindowSize(int32_t* w, int32_t* h) {
  if (w)
    (*w) = this.windowWidth;

  if (h)
    (*h) = this.windowHeight;
}


int32_t Renderer_LoadTexture(const char* path) {
  int w, h, n;
  uint8_t* data = stbi_load(path, &w, &h, &n, 0);

  if (!data) {
    Engine_Error("Unable to create texture '%s'", path);
    return -1;
  }

  struct stbrp_rect rect = { 0, w, h };
  stbrp_pack_rects(&this.rectPackCtx, &rect, 1);
  if (!rect.was_packed) {
    Engine_Error("Unable to pack texture into atlas");
    return -1;
  }

  glGetError();
  glBindTexture(GL_TEXTURE_2D, this.atlasID);
  glTexSubImage2D(GL_TEXTURE_2D, 0, rect.x, rect.y, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
  glBindTexture(GL_TEXTURE_2D, 0);
  stbi_image_free(data);

  for (int32_t i = 0; i < MAX_TEXTURES; i++) {
    Texture_t* texture = &this.textures[i];

    if (texture->state == TEXTURESTATE_EMPTY) {
      texture->state = TEXTURESTATE_USED;
      texture->rect.x = rect.x;
      texture->rect.y = rect.y;
      texture->rect.w = rect.w;
      texture->rect.h = rect.h;

      return i;
    }
  }

  return -1;
}


void Renderer_FreeTexture(int32_t textureID) {

}


void Renderer_PushAtlas() {
  Vert_t* vert;

  color_t color = { 0xff, 0xff, 0xff, 0xff };

  vert = &this.vertices[this.currVert];
  vert->x = 0;
  vert->y = 0;
  vert->u = 0;
  vert->v = 0;
  vert->color = color;

  vert = &this.vertices[this.currVert+1];
  vert->x = 1024;
  vert->y = 0;
  vert->u = MAX_ATLAS_DIMENSIONS;
  vert->v = 0;
  vert->color = color;

  vert = &this.vertices[this.currVert+2];
  vert->x = 1024;
  vert->y = 1024;
  vert->u = MAX_ATLAS_DIMENSIONS;
  vert->v = MAX_ATLAS_DIMENSIONS;
  vert->color = color;

  vert = &this.vertices[this.currVert+3];
  vert->x = 0;
  vert->y = 1024;
  vert->u = 0;
  vert->v = MAX_ATLAS_DIMENSIONS;
  vert->color = color;

  this.indices[this.currIndex++] = this.currVert;
  this.indices[this.currIndex++] = this.currVert+1;
  this.indices[this.currIndex++] = this.currVert+2;
  this.indices[this.currIndex++] = this.currVert+2;
  this.indices[this.currIndex++] = this.currVert+3;
  this.indices[this.currIndex++] = this.currVert;

  this.currVert += 4;
}

int32_t Renderer_PushTexturedRect(int32_t textureID, rect_t* dst, rect_t* src) {
  // Define shape of drawing destination
  rect_t newDst;
  if (dst) {
    newDst = (*dst);
  } else {
    newDst.x = 0;
    newDst.y = 0;
    newDst.w = this.windowWidth;
    newDst.h = this.windowHeight;
  }

  // Get UV coordinates from textureID
  vec2f_t minTexCoords, maxTexCoords;
  if (textureID < 0 || textureID >= MAX_TEXTURES) {
    minTexCoords = (vec2f_t){ 65535.0f, 65535.0f };
    maxTexCoords = (vec2f_t){ 65535.0f, 65535.0f };
  } else {
    Texture_t* texture = &this.textures[textureID];
    if (src) {
      minTexCoords = (vec2f_t){
        (texture->rect.x + src->x),
        (texture->rect.y + src->y)
      };
      maxTexCoords = (vec2f_t){
        (texture->rect.x + src->x + src->w),
        (texture->rect.y + src->y + src->h)
      };
    } else {
      minTexCoords = (vec2f_t){ texture->rect.x,                 texture->rect.y                 };
      maxTexCoords = (vec2f_t){ texture->rect.x+texture->rect.w, texture->rect.y+texture->rect.h };
    }
  }

  const color_t noColor = { 0xff, 0xff, 0xff, 0xff };

  Vert_t* verts = &this.vertices[this.currVert];
  verts[0] = (Vert_t){ newDst.x,          newDst.y,          minTexCoords.x, minTexCoords.y, noColor };
  verts[1] = (Vert_t){ newDst.x+newDst.w, newDst.y,          maxTexCoords.x, minTexCoords.y, noColor };
  verts[2] = (Vert_t){ newDst.x+newDst.w, newDst.y+newDst.h, maxTexCoords.x, maxTexCoords.y, noColor };
  verts[3] = (Vert_t){ newDst.x,          newDst.y+newDst.h, minTexCoords.x, maxTexCoords.y, noColor };


  this.indices[this.currIndex++] = this.currVert;
  this.indices[this.currIndex++] = this.currVert+1;
  this.indices[this.currIndex++] = this.currVert+2;
  this.indices[this.currIndex++] = this.currVert+2;
  this.indices[this.currIndex++] = this.currVert+3;
  this.indices[this.currIndex++] = this.currVert;

  this.currVert += 4;

  return this.currVert - 4;
}


int32_t Renderer_PushRect(rect_t* rect, color_t color) {
  if (this.currVert+4 >= MAX_VERTICES) {
    Engine_Error("Too many vertices in buffer");
    return -1;
  }

  rect_t newRect;
  if (rect) {
    newRect = (*rect);
  } else {
    newRect.x = 0;
    newRect.y = 0;
    newRect.w = this.windowWidth;
    newRect.h = this.windowHeight;
  }

  int32_t index = this.currVert;

  Vert_t* verts = &this.vertices[this.currVert];
  verts[0] = (Vert_t){ newRect.x,           newRect.y,           -1, -1, color };
  verts[1] = (Vert_t){ newRect.x+newRect.w, newRect.y,           -1, -1, color };
  verts[2] = (Vert_t){ newRect.x+newRect.w, newRect.y+newRect.h, -1, -1, color };
  verts[3] = (Vert_t){ newRect.x,           newRect.y+newRect.h, -1, -1, color };

  this.indices[this.currIndex++] = this.currVert;
  this.indices[this.currIndex++] = this.currVert+1;
  this.indices[this.currIndex++] = this.currVert+2;
  this.indices[this.currIndex++] = this.currVert+2;
  this.indices[this.currIndex++] = this.currVert+3;
  this.indices[this.currIndex++] = this.currVert;

  this.currVert += 4;

  return index;
}


void Renderer_RotateRect(int32_t index, rect_t* rect, vec2f_t center, float angle) {
  if (index < 0 || index >= MAX_VERTICES)
    return;

  if (!rect)
    return;

  float cx = rect->x + (center.x * rect->w);
  float cy = rect->y + (center.y * rect->h);

  for (int32_t i = index; i < index+4; i++) {
    float vertX = (float)this.vertices[i].x - cx;
    float vertY = (float)this.vertices[i].y - cy;
    float px = vertX * cos(angle) - vertY * sin(angle);
    float py = vertX * sin(angle) + vertY * cos(angle);
    this.vertices[i].x = px + cx;
    this.vertices[i].y = py + cy;
  }
}


void Renderer_SetVertColors(int32_t index, int32_t len, color_t* colors) {
  if (index < 0 || index >= MAX_VERTICES)
    return;

  for (int32_t i = index, j = 0; j < 4; i++, j++) {
    this.vertices[i].color = colors[j];
  }
}
