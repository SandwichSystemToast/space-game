#pragma once

#include "../def.h"

#include <flecs.h>

typedef struct {
  v2 position;
  float rotation;
} c_transform;

ECS_COMPONENT_DECLARE(c_transform);
