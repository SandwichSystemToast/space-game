#pragma once

#include <flecs/flecs.h>

#include "raylib.h"

#include "../player/character.h"
#include "camera.h"

void begin_frame(ecs_iter_t *it) {
  c_player_character *character = ecs_field(it, c_player_character, 1);
  const c_camera *cam = ecs_field(it, c_camera, 2);
  Camera2D cam2d = cam->cam2d;

  if (it->count == 0)
    return;
  EXPECT(it->count == 1, "Too many players to draw");

  f32 mouse_look_weight = 0.1;

  // TODO: deduce if the camera should look at the ship or something else
  cam2d.rotation = 0.;
  cam2d.zoom = 1.2;
  cam2d.offset.x = GetScreenWidth() / 2.;
  cam2d.offset.y = GetScreenHeight() / 2.;
  cam2d.target =
      Vector2Add(Vector2Scale(Vector2Subtract(GetMousePosition(), cam2d.offset),
                              mouse_look_weight),
                 character->position);

  BeginMode2D(cam2d);
}

void end_frame(ecs_iter_t *it) { EndDrawing(); }
