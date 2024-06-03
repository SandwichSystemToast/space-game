#pragma once

#include "../core/transform.h"
#include "../def.h"

#include "flecs.h"

#include "raylib.h"
#include "raymath.h"

#include <math.h>

// TODO: is it ok if this is an int?
// used to ensure that at least UNITS_PER_MINIMAL_DIMENSION^2
// in-game units are visible at any time
#define UNITS_PER_MINIMAL_DIMENSION 1
#define PIXELS_PER_UNIT 64
#define UNITS_PER_PIXEL (1 / (f32)PIXELS_PER_UNIT)
#define MIN_ZOOM 0.3f
#define MAX_ZOOM 3.f

typedef struct {
  Camera2D cam2d;

  f32 zoom;
  f32 mouse_look_weight;
  f32 target_aspect_ratio;

  ecs_entity_t look_at;
} c_camera;

ECS_COMPONENT_DECLARE(c_camera);

c_camera camera_new() {
  c_camera ret;
  ret.cam2d.offset = Vector2Zero();
  ret.cam2d.rotation = 0.;
  ret.cam2d.zoom = 1.;
  ret.cam2d.target = Vector2Zero();

  ret.zoom = 1.;
  ret.target_aspect_ratio = 16. / 9.;
  ret.mouse_look_weight = 0.2;

  return ret;
}

v2 c_camera_relative_mouse_position(const c_camera *cam) {
  return Vector2Scale(Vector2Subtract(GetMousePosition(), cam->cam2d.offset),
                      1. / cam->cam2d.zoom);
}

v2 c_camera_world_mouse_position(const c_camera *cam) {
  return Vector2Add(c_camera_relative_mouse_position(cam), cam->cam2d.target);
}

void camera_follow(ecs_iter_t *it) {
  c_camera *cam = ecs_field(it, c_camera, 1);

  EXPECT(ecs_is_valid(it->world, cam->look_at),
         "The camera target is expected to be alive");
  EXPECT(ecs_has(it->world, cam->look_at, c_transform),
         "The camera target does not have a transform component");
  const c_transform *transform = ecs_get(it->world, cam->look_at, c_transform);

  // TODO: consider HiDPI
  f32 actual_aspect_ratio = (f32)GetRenderWidth() / (f32)GetRenderHeight();
  f32 pixels_to_units_zoom =
      (fminf(GetRenderWidth(), GetRenderHeight()) / PIXELS_PER_UNIT) /
      UNITS_PER_MINIMAL_DIMENSION;

  cam->zoom = Clamp(cam->zoom + GetMouseWheelMove(), MIN_ZOOM, MAX_ZOOM);

  f32 desired_zoom = cam->zoom * pixels_to_units_zoom * actual_aspect_ratio /
                     cam->target_aspect_ratio;

  // TODO: deduce if the camera should look at the ship or something else
  cam->target_aspect_ratio = 16. / 9.;
  cam->mouse_look_weight = 0.2;

  Camera2D *cam2d = &cam->cam2d;
  cam2d->rotation = 0.;
  cam2d->zoom = desired_zoom;
  cam2d->offset.x = GetScreenWidth() / 2.;
  cam2d->offset.y = GetScreenHeight() / 2.;

  v2 mouse_pos = c_camera_relative_mouse_position(cam);
  cam2d->target = Vector2Add(Vector2Scale(mouse_pos, cam->mouse_look_weight),
                             transform->position);
}
