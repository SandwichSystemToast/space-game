#include "def.h"

#include "flecs.h"
#include "flecs/addons/flecs_c.h"
#include "raylib.h"
#include "raymath.h"

#include "core/transform.h"
#include "physics/collisions.h"
#include "physics/quadtree.h"
#include "physics/shape.h"
#include "player/character.h"
#include "player/input.h"
#include "rendering/camera.h"
#include "rendering/rendering.h"

typedef struct {
  f32 radius;
} c_asteroid;

ECS_COMPONENT_DECLARE(c_asteroid);

void render_player(ecs_iter_t *it) {
  c_player_character *character = ecs_field(it, c_player_character, 1);
  const c_player_input *input = ecs_field(it, c_player_input, 2);
  const c_camera *cam = ecs_field(it, c_camera, 3);
  const c_transform *transform =
      ecs_get(it->real_world, cam->look_at, c_transform);

  if (it->count == 0)
    return;

  EXPECT(it->count == 1, "Too many players to draw");

  DrawLineV(
      transform->position,
      Vector2Add(transform->position, Vector2Scale(input->direction, 40.)),
      BLUE);
  DrawLineV(transform->position,
            Vector2Add(transform->position, character->velocity), RED);

  DrawLineV(transform->position, c_camera_world_mouse_position(cam), BLUE);
  DrawCircleV(transform->position, 1.5, WHITE);
}

void render_shapes(ecs_iter_t *it) {
  c_transform *transform = ecs_field(it, c_transform, 1);
  c_physics_shape *shape = ecs_field(it, c_physics_shape, 2);

  for (z i = 0; i < it->count; i++) {
    for (z j = 0; j < shape[i].vertex_count; j++) {
      z j1 = j, j2 = (j + 1) % shape[i].vertex_count;

      DrawLineV(c_transform_vector(&transform[i], shape[i].vertices[j1]),
                c_transform_vector(&transform[i], shape[i].vertices[j2]), RED);
    }
  }
}

int main(void) {
  ecs_world_t *world = ecs_init();
  init_root(512);
  printf("ROOT %d\n", root);

  // Init a rest debugger and a statistics monitor
#ifndef NDEBUG
  ecs_singleton_set(world, EcsRest, {0});
  ECS_IMPORT(world, FlecsMonitor);
#endif

  // singletons
  ECS_COMPONENT_DEFINE(world, c_player_input);
  ECS_COMPONENT_DEFINE(world, c_camera);

  // components
  ECS_COMPONENT_DEFINE(world, c_transform);
  ECS_COMPONENT_DEFINE(world, c_player_character);
  ECS_COMPONENT_DEFINE(world, c_asteroid);
  ECS_COMPONENT_DEFINE(world, c_physics_shape);

  // systems
  ECS_SYSTEM(world, accept_input, EcsOnLoad, c_player_input($));
  ECS_SYSTEM(world, begin_frame, EcsPostLoad, c_camera($));

  ECS_SYSTEM(world, camera_follow, EcsPreUpdate, c_camera($));

  ECS_SYSTEM(world, move_player_character, EcsOnUpdate, c_player_character,
             c_transform, c_player_input($));

  ECS_SYSTEM(world, solve_collisions, EcsOnUpdate);

  ECS_SYSTEM(world, render_player, EcsPostUpdate, c_player_character,
             c_player_input($), c_camera($));
  ECS_SYSTEM(world, render_shapes, EcsPostUpdate, c_transform, c_physics_shape);

  // ECS_SYSTEM(world, end_frame, EcsOnStore);

  // singletons
  ecs_singleton_set(world, c_player_input, {0});
  ecs_singleton_set(world, c_camera, {});

  // player
  ecs_entity_t player = ecs_new_id(world);
  ecs_set(world, player, c_player_character, {});
  ecs_set(world, player, c_transform, {});
  ecs_set(world, player, c_physics_shape, {});

  c_physics_shape *player_shape = ecs_get(world, player, c_physics_shape);
  c_physics_shape_circle_init(player_shape, 1.5, 32);

  c_transform *transform = ecs_get(world, player, c_transform);
  transform->position.x = 0;
  transform->position.y = 0;

  // asteroid 1
  ecs_entity_t asteroid1 = ecs_new_id(world);
  ecs_set(world, asteroid1, c_asteroid, {});
  ecs_set(world, asteroid1, c_physics_shape, {});
  ecs_set(world, asteroid1, c_transform, {});
  c_transform *asteroid1_transform = ecs_get(world, asteroid1, c_transform);
  asteroid1_transform->position.x = 1.5;
  asteroid1_transform->position.y = 1.5;

  c_physics_shape *asteroid1_shape = ecs_get(world, asteroid1, c_physics_shape);
  c_physics_shape_circle_init(asteroid1_shape, 4., 16);

  // asteroid 2
  ecs_entity_t asteroid2 = ecs_new_id(world);
  ecs_set(world, asteroid2, c_asteroid, {});
  ecs_set(world, asteroid2, c_physics_shape, {});
  ecs_set(world, asteroid2, c_transform, {});

  c_physics_shape *asteroid2_shape = ecs_get(world, asteroid2, c_physics_shape);
  c_physics_shape_circle_init(asteroid2_shape, 8.9, 64);

  c_transform *asteroid2_transform = ecs_get(world, asteroid2, c_transform);
  asteroid2_transform->position.x = 7.5;
  asteroid2_transform->position.y = 7.5;

  // camera
  c_camera *cam = ecs_singleton_get(world, c_camera);
  cam->look_at = player;

  InitWindow(800, 450, "Space Game");
  SetWindowState(FLAG_WINDOW_RESIZABLE);

  SetTargetFPS(60);

  add_entity(root, asteroid1, world);
  add_entity(root, asteroid2, world);
  add_entity(root, player, world);

  srand(0xb00b);
  for (int i = 0; i < 500; i++) {
    ecs_entity_t aaaa = ecs_new_id(world);
    ecs_set(world, aaaa, c_asteroid, {});
    ecs_set(world, aaaa, c_physics_shape, {});
    ecs_set(world, aaaa, c_transform, {});
    c_transform *aaaa_transform = ecs_get(world, aaaa, c_transform);
    aaaa_transform->position.x = (float)rand() / RAND_MAX * 200.f;
    aaaa_transform->position.y = (float)rand() / RAND_MAX * 200.f;
    add_entity(root, aaaa, world);
  }

  while (!WindowShouldClose()) {
    ecs_progress(world, GetFrameTime());
    render_quadtree(root, world);
    EndDrawing();
  }

  CloseWindow();

  ecs_fini(world);

  return 0;
}
