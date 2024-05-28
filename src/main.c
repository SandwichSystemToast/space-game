#include "def.h"

#include "raylib.h"

#define PHYSAC_IMPLEMENTATION
#include "physac/src/physac.h"
#undef PHYSAC_IMPLEMENTATION

#include "flecs/flecs.h"

int main(void) {
  ecs_world_t *world = ecs_init();
  InitPhysics();
  InitWindow(800, 450, "raylib [core] example - basic window");

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawText("Congrats! You created your first window!", 190, 200, 20,
             LIGHTGRAY);
    EndDrawing();
  }

  CloseWindow();
  ClosePhysics();
  ecs_fini(world);

  return 0;
}
