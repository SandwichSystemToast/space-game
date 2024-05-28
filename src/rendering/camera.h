#pragma once

#include "../def.h"
#include "../player/character.h"
#include "flecs.h"
#include "raylib.h"

#include "flecs/flecs.h"
#include "raymath.h"

typedef struct {
  Camera2D cam2d;
  float mouse_look_weight;
} c_camera;

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
  c_player_character *character = ecs_field(it, c_player_character, 1);
  c_camera *cam = ecs_field(it, c_camera, 2);

  EXPECT(it->count != 0, "No cameras in the scene, impossible to follow");
  EXPECT(it->count == 1, "More than one camera at a time");

  // TODO: deduce if the camera should look at the ship or something else
  Camera2D *cam2d = &cam->cam2d;
  cam2d->rotation = 0.;
  cam2d->zoom = 1.2;
  cam2d->offset.x = GetScreenWidth() / 2.;
  cam2d->offset.y = GetScreenHeight() / 2.;
  cam->mouse_look_weight = 0.2;

  v2 mouse_pos = c_camera_relative_mouse_position(cam);
  cam2d->target = Vector2Add(Vector2Scale(mouse_pos, cam->mouse_look_weight),
                             character->position);
}
