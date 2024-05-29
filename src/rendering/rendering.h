#pragma once

#include <flecs/flecs.h>

#include "raylib.h"

#include "camera.h"

void begin_frame(ecs_iter_t *it) {
  const c_camera *cam = ecs_field(it, c_camera, 1);
  Camera2D cam2d = cam->cam2d;

  BeginMode2D(cam2d);
  ClearBackground(BLACK);
}

void end_frame(ecs_iter_t *it) { EndDrawing(); }
