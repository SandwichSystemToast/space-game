#include "def.h"

#include "raylib.h"
#include "raymath.h"

#define PHYSAC_IMPLEMENTATION
#include "physac/src/physac.h"
#undef PHYSAC_IMPLEMENTATION

#include "flecs/flecs.h"

#include "player/character.h"
#include "player/input.h"

void render_player(ecs_iter_t *it) {
  player_character *character = ecs_field(it, player_character, 1);
  const player_input *input = ecs_field(it, player_input, 2);

  if (it->count == 0)
    return;

  EXPECT(it->count == 1, "Too many players to draw");

  f32 mouse_look_weight = 0.1;

  Camera2D cam;
  cam.rotation = 0.;
  cam.zoom = 1.2;
  cam.offset.x = GetScreenWidth() / 2.;
  cam.offset.y = GetScreenHeight() / 2.;
  cam.target =
      Vector2Add(Vector2Scale(Vector2Subtract(GetMousePosition(), cam.offset),
                              mouse_look_weight),
                 character->position);

  BeginMode2D(cam);
  ClearBackground(BLACK);

  DrawLineV(
      character->position,
      Vector2Add(character->position, Vector2Scale(input->direction, 40.)),
      BLUE);
  DrawLineV(character->position,
            Vector2Add(character->position, character->velocity), RED);

#define FOV_ANGLE 40. * DEG2RAD
  // Vision Cone
  DrawLineV(character->position,
            Vector2Add(Vector2Scale(
                           Vector2Normalize(Vector2Rotate(
                               Vector2Subtract(cam.target, character->position),
                               FOV_ANGLE)),
                           10000.),
                       character->position),
            GRAY);

  DrawLineV(character->position,
            Vector2Add(Vector2Scale(
                           Vector2Normalize(Vector2Rotate(
                               Vector2Subtract(cam.target, character->position),
                               -FOV_ANGLE)),
                           10000.),
                       character->position),
            GRAY);

  DrawCircleV(Vector2Zero(), 10, GREEN);

  DrawCircleV(character->position, 15., WHITE);

  EndDrawing();
}

int main(void) {
  ecs_world_t *world = ecs_init();

  ECS_COMPONENT(world, player_input);
  ECS_COMPONENT(world, player_character);

  ECS_SYSTEM(world, move_player_character, EcsOnUpdate, player_character,
             player_input($));
  ECS_SYSTEM(world, accept_input, EcsOnUpdate, player_input($));
  ECS_SYSTEM(world, render_player, EcsOnUpdate, player_character,
             player_input($));

  ecs_singleton_set(world, player_input, {0});
  ecs_entity_t player = ecs_new_id(world);
  ecs_set(world, player, player_character, {});

  InitPhysics();
  InitWindow(800, 450, "Space Game");

  SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

  while (!WindowShouldClose()) {
    ecs_progress(world, GetFrameTime());
  }

  CloseWindow();
  ClosePhysics();

  ecs_fini(world);

  return 0;
}
