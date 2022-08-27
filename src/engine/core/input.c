#include "input.h"

static Input_t this;

// Initialize input state
bool Input_Init() {
  memset(&this, 0, sizeof(Input_t));

  return true;
}


// Cleanup input state
void Input_Cleanup() {

}


// Get the input for this frame and return if the program should continue running
bool Input_GetInput() {
  bool running = true;

  memcpy(&this.prev, &this.curr, sizeof(VirtualController_t));

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch(event.type) {
    case SDL_QUIT:
      running = false;
      break;
    case SDL_KEYDOWN:
      if (event.key.keysym.sym == SDLK_w)
        Renderer_ToggleWireframe();
      break;
    }
  }

  return running;
}
