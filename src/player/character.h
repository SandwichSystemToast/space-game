#pragma once

#include "../core/transform.h"
#include "../def.h"
#include "flecs/flecs.h"

#include "input.h"

#define CHARACTER_MAX_SPEED 240.
#define CHARACTER_MAX_ACCELERATION (10. * CHARACTER_MAX_SPEED)
#define CHARACTER_DRAG 10.

typedef struct player_character {
  v2 velocity;
} c_player_character;

ECS_COMPONENT_DECLARE(c_player_character);

void move_player_character(ecs_iter_t *it) {
  f32 dt = it->delta_time;

  c_player_character *character = ecs_field(it, c_player_character, 1);
  c_transform *transform = ecs_field(it, c_transform, 2);
  const c_player_input *input = ecs_field(it, c_player_input, 3);

  EXPECT(it->count <= 1,
         "More than two players controlled at a time, possibly a bug");

  character->velocity =
      Vector2Scale(character->velocity, (1 - CHARACTER_DRAG * dt));

  f32 current_speed = Vector2DotProduct(character->velocity, input->direction);
  if (current_speed > CHARACTER_MAX_SPEED)
    current_speed = CHARACTER_MAX_SPEED;
  f32 acceleration = CHARACTER_MAX_SPEED - current_speed;
  if (acceleration < 0.)
    acceleration = 0.;
  else if (acceleration > CHARACTER_MAX_ACCELERATION * dt)
    acceleration = CHARACTER_MAX_ACCELERATION * dt;

  character->velocity = Vector2Add(
      character->velocity, Vector2Scale(input->direction, acceleration));
  transform->position =
      Vector2Add(transform->position, Vector2Scale(character->velocity, dt));
}
