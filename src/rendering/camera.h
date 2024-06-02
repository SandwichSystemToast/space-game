#pragma once

#include "../core/transform.h"
#include "../def.h"
#include "flecs.h"
#include "raylib.h"

#include "flecs/flecs.h"
#include "raymath.h"

typedef struct {
  Camera2D cam2d;

  float mouse_look_weight;
  float ease_lerp_t;

  ecs_entity_t look_at;
} c_camera;

ECS_COMPONENT_DECLARE(c_camera);

c_camera camera_new() {
  c_camera ret;
  ret.cam2d.offset = Vector2Zero();
  ret.cam2d.rotation = 0.;
  ret.cam2d.zoom = 1.;
  ret.cam2d.target = Vector2Zero();
  ret.mouse_look_weight = 0.1;
  return ret;
}

v2 c_camera_relative_mouse_position(const c_camera *cam) {
  return Vector2Subtract(GetMousePosition(), cam->cam2d.offset);
}

void camera_follow(ecs_iter_t *it) {
  f32 dt = it->delta_time;
  c_camera *cam = ecs_field(it, c_camera, 1);

  EXPECT(ecs_is_valid(it->world, cam->look_at),
         "The camera target is expected to be alive");
  EXPECT(ecs_has(it->world, cam->look_at, c_transform),
         "The camera target does not have a transform component");
  const c_transform *transform = ecs_get(it->world, cam->look_at, c_transform);

  // TODO: deduce if the camera should look at the ship or something else
  Camera2D *cam2d = &cam->cam2d;
  cam2d->rotation = 0.;
  cam2d->zoom = 1.2;
  cam2d->offset.x = GetScreenWidth() / 2.;
  cam2d->offset.y = GetScreenHeight() / 2.;
  cam->mouse_look_weight = 0.2;
  cam->ease_lerp_t = 0.999;

  v2 mouse_pos = c_camera_relative_mouse_position(cam);
  v2 camera_new_target = Vector2Add(
      Vector2Scale(mouse_pos, cam->mouse_look_weight), transform->position);

  v2 eased_target =
      Vector2Add(Vector2Scale(Vector2Subtract(cam2d->target, camera_new_target),
                              powf(1. - cam->ease_lerp_t, dt)),
                 camera_new_target);

  cam2d->target = eased_target;
}
