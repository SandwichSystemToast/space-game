#pragma once

#include "../def.h"
#include "flecs/flecs.h"

#include "raylib.h"
#include "raymath.h"

typedef struct {
  v2 direction;
} c_player_input;

void accept_input(ecs_iter_t *it) {
  c_player_input *input = ecs_field(it, c_player_input, 1);

  input->direction = Vector2Zero();
  if (IsKeyDown(KEY_D))
    input->direction.x += 1.;
  if (IsKeyDown(KEY_A))
    input->direction.x -= 1;
  if (IsKeyDown(KEY_W))
    input->direction.y -= 1;
  if (IsKeyDown(KEY_S))
    input->direction.y += 1;
  input->direction = Vector2Normalize(input->direction);
}
