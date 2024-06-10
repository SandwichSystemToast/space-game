#pragma once

#include "../def.h"

#include <flecs.h>
#include <raymath.h>

typedef struct {
  v2 local_position;
  f32 rotation;
} c_transform;

v2 c_transform_vector(c_transform *transform, v2 vector) {
  return Vector2Add(Vector2Rotate(vector, transform->rotation),
                    transform->local_position);
}

void c_transform_get_rounded_pos(c_transform *transform, i64 *x, i64 *y) {
  *x = (i64)ceilf(transform->local_position.x);
  *y = (i64)ceilf(transform->local_position.y);
}

ECS_COMPONENT_DECLARE(c_transform);
