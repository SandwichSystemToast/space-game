#include "def.h"

#include "raylib.h"
#include "raymath.h"

#define PHYSAC_IMPLEMENTATION
#include "physac/src/physac.h"
#undef PHYSAC_IMPLEMENTATION

#include "flecs/flecs.h"

#include "player/character.h"
#include "player/input.h"

int main(void) {
  ecs_world_t *world = ecs_init();

  ECS_COMPONENT(world, player_input);
  ECS_COMPONENT(world, player_character);

  ECS_SYSTEM(world, move_player_character, EcsOnUpdate, player_character,
             player_input($));
  ECS_SYSTEM(world, accept_input, EcsOnUpdate, player_input($));

  ecs_singleton_set(world, player_input, {0});
  ecs_entity_t player = ecs_new_id(world);
  ecs_set(world, player, player_character, {});

  InitPhysics();
  InitWindow(800, 450, "raylib [core] example - basic window");

  // https://github.com/id-Software/Quake/blob/master/WinQuake/sv_user.c#L190
  SetTargetFPS(60);

  f32 mouse_look_weight = 0.1;
  SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

  while (!WindowShouldClose()) {
    ecs_progress(world, GetFrameTime());
    
    const player_character *character =
        ecs_get(world, player, player_character);
    const player_input *input = ecs_singleton_get(world, player_input);

    v2 position = character->position;
    v2 velocity = character->velocity;

    Camera2D cam;
    cam.rotation = 0.;
    cam.zoom = 1.2;
    cam.offset.x = GetScreenWidth() / 2.;
    cam.offset.y = GetScreenHeight() / 2.;
    cam.target =
        Vector2Add(Vector2Scale(Vector2Subtract(GetMousePosition(), cam.offset),
                                mouse_look_weight),
                   position);

    BeginMode2D(cam);
    ClearBackground(BLACK);

    DrawLineV(position,
              Vector2Add(position, Vector2Scale(input->direction, 40.)), BLUE);
    DrawLineV(position, Vector2Add(position, velocity), RED);

#define FOV_ANGLE 40. * DEG2RAD
    // Vision Cone
    DrawLineV(position,
              Vector2Add(Vector2Scale(Vector2Normalize(Vector2Rotate(
                                          Vector2Subtract(cam.target, position),
                                          FOV_ANGLE)),
                                      10000.),
                         position),
              GRAY);

    DrawLineV(position,
              Vector2Add(Vector2Scale(Vector2Normalize(Vector2Rotate(
                                          Vector2Subtract(cam.target, position),
                                          -FOV_ANGLE)),
                                      10000.),
                         position),
              GRAY);

    DrawCircleV(Vector2Zero(), 10, GREEN);

    DrawCircleV(position, 15., WHITE);

    EndDrawing();
  }

  CloseWindow();
  ClosePhysics();

  ecs_fini(world);

  return 0;
}
