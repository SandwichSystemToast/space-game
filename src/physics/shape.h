#pragma once

#include "../def.h"

#include <flecs.h>

typedef struct {
  v2* vertices;
  z vertex_count;
} c_physics_shape;

ECS_COMPONENT_DECLARE(c_physics_shape);
