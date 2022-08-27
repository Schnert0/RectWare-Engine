#include "main.h"

int main(int argc, char* argv[]) {
  if (Engine_Init()) {
    Engine_Run();
  }

  Engine_Cleanup();

  return 0;
}
