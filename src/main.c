#include "def.h"

#include "flecs/addons/flecs_c.h"
#include "raylib.h"
#include "raymath.h"

#define PHYSAC_IMPLEMENTATION
#include "physac/src/physac.h"
#undef PHYSAC_IMPLEMENTATION

#include "core/transform.h"
#include "physics/collisions.h"
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

#define FOV_ANGLE 40. * DEG2RAD

  // Vision Cone
  DrawLineV(transform->position,
            Vector2Add(Vector2Scale(Vector2Normalize(Vector2Rotate(
                                        Vector2Subtract(cam->cam2d.target,
                                                        transform->position),
                                        FOV_ANGLE)),
                                    10000.),
                       transform->position),
            GRAY);

  DrawLineV(transform->position,
            Vector2Add(Vector2Scale(Vector2Normalize(Vector2Rotate(
                                        Vector2Subtract(cam->cam2d.target,
                                                        transform->position),
                                        -FOV_ANGLE)),
                                    10000.),
                       transform->position),
            GRAY);

  DrawCircleV(transform->position, 15., WHITE);
}

void render_shapes(ecs_iter_t *iterator) {
  ecs_filter_t *f =
      ecs_filter(iterator->world,
                 {.terms = {{ecs_id(c_transform)}, {ecs_id(c_physics_shape)}}});
  ecs_iter_t it = ecs_filter_iter(iterator->world, f);

  while (ecs_filter_next(&it)) {
    for (int i = 0; i < it.count; i++) {
      c_transform *transform = ecs_field(&it, c_transform, 1);
      c_physics_shape *shape = ecs_field(&it, c_physics_shape, 2);

      for (z j = 0; j < shape->vertex_count; j++) {
        z j1 = j;
        z j2 = (j + 1) % shape->vertex_count;

        DrawLineV(Vector2Add(transform->position, shape->vertices[j1]),
                  Vector2Add(transform->position, shape->vertices[j2]), RED);
      }
    }
  }
}

int main(void) {
  ecs_world_t *world = ecs_init();

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
  ECS_SYSTEM(world, move_player_character, EcsOnUpdate, c_player_character,
             c_transform, c_player_input($));

  ECS_SYSTEM(world, accept_input, EcsOnUpdate, c_player_input($));
  ECS_SYSTEM(world, camera_follow, EcsPreUpdate, c_camera($));
  ECS_SYSTEM(world, begin_frame, EcsPreUpdate, c_camera($));

  ECS_SYSTEM(world, solve_collisions, EcsOnUpdate, c_transform,
             c_physics_shape);

  ECS_SYSTEM(world, render_player, EcsOnUpdate, c_player_character,
             c_player_input($), c_camera($));
  ECS_SYSTEM(world, render_shapes, EcsOnUpdate);

  ECS_SYSTEM(world, end_frame, EcsPostUpdate);

  // singletons
  ecs_singleton_set(world, c_player_input, {0});
  ecs_singleton_set(world, c_camera, {});

  // player
  ecs_entity_t player = ecs_new_id(world);
  ecs_set(world, player, c_player_character, {});
  ecs_set(world, player, c_transform, {});
  ecs_set(world, player, c_physics_shape, {});

  c_physics_shape *player_shape = ecs_get(world, player, c_physics_shape);
  c_physics_shape_circle_init(player_shape, 15., 32);

  c_transform *transform = ecs_get(world, player, c_transform);
  transform->position.x = -75;
  transform->position.y = -75;

  // asteroid
  ecs_entity_t asteroid = ecs_new_id(world);
  ecs_set(world, asteroid, c_asteroid, {});
  ecs_set(world, asteroid, c_physics_shape, {});
  ecs_set(world, asteroid, c_transform, {});

  c_physics_shape *shape = ecs_get(world, asteroid, c_physics_shape);
  c_physics_shape_circle_init(shape, 40., 16);

  // camera

  c_camera *cam = ecs_singleton_get(world, c_camera);
  cam->look_at = player;

  InitPhysics();
  InitWindow(800, 450, "Space Game");

  while (!WindowShouldClose()) {
    ecs_progress(world, GetFrameTime());
  }

  CloseWindow();
  ClosePhysics();

  ecs_fini(world);

  return 0;
}
