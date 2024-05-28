#include "def.h"

#include "raylib.h"
#include "raymath.h"

#define PHYSAC_IMPLEMENTATION
#include "physac/src/physac.h"
#undef PHYSAC_IMPLEMENTATION

#include "player/character.h"
#include "player/input.h"
#include "rendering/camera.h"
#include "rendering/rendering.h"

void render_player(ecs_iter_t *it) {
  c_player_character *character = ecs_field(it, c_player_character, 1);
  const c_player_input *input = ecs_field(it, c_player_input, 2);
  const c_camera *cam = ecs_field(it, c_camera, 3);

  if (it->count == 0)
    return;

  EXPECT(it->count == 1, "Too many players to draw");

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
            Vector2Add(Vector2Scale(Vector2Normalize(Vector2Rotate(
                                        Vector2Subtract(cam->cam2d.target,
                                                        character->position),
                                        FOV_ANGLE)),
                                    10000.),
                       character->position),
            GRAY);

  DrawLineV(character->position,
            Vector2Add(Vector2Scale(Vector2Normalize(Vector2Rotate(
                                        Vector2Subtract(cam->cam2d.target,
                                                        character->position),
                                        -FOV_ANGLE)),
                                    10000.),
                       character->position),
            GRAY);

  DrawCircleV(Vector2Zero(), 10, GREEN);

  DrawCircleV(character->position, 15., WHITE);
}

int main(void) {
  ecs_world_t *world = ecs_init();

  ECS_COMPONENT(world, c_player_input);
  ECS_COMPONENT(world, c_player_character);
  ECS_COMPONENT(world, c_camera);

  ECS_SYSTEM(world, move_player_character, EcsOnUpdate, c_player_character,
             c_player_input($));

  ECS_SYSTEM(world, accept_input, EcsOnUpdate, c_player_input($));
  ECS_SYSTEM(world, camera_follow, EcsPreUpdate, c_player_character,
             c_camera($));
  ECS_SYSTEM(world, begin_frame, EcsPreUpdate, c_player_character, c_camera($));
  ECS_SYSTEM(world, render_player, EcsOnUpdate, c_player_character,
             c_player_input($), c_camera($));

  ECS_SYSTEM(world, end_frame, EcsPostUpdate);

  // singletons
  ecs_singleton_set(world, c_player_input, {0});
  ecs_singleton_set(world, c_camera, {});

  ecs_entity_t player = ecs_new_id(world);
  ecs_set(world, player, c_player_character, {});

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
