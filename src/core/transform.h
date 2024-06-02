#pragma once

#include "../def.h"

#include <flecs.h>
#include <raymath.h>

typedef struct {
  v2 position;
  f32 rotation;
} c_transform;

v2 c_transform_vector(c_transform *transform, v2 vector) {
  return Vector2Add(Vector2Rotate(vector, transform->rotation),
                    transform->position);
}

ECS_COMPONENT_DECLARE(c_transform);
