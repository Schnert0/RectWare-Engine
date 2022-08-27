#ifndef INPUT_H
#define INPUT_H

#include "../../common.h"
#include "../engine.h"

typedef struct {
  vec2f_t pos;
  float angle, magnetude;
} VirtualAnalogStick_t;

typedef struct {
  VirtualAnalogStick_t leftStick, rightStick;
  int8_t leftTrigger, rightTrigger;
  uint32_t buttons;
} VirtualController_t;

typedef struct {
  VirtualController_t prev, curr;
} Input_t;

bool Input_Init();
void Input_Cleanup();

bool Input_GetInput();

#endif // INPUT_H
