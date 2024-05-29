#pragma once

#include "../def.h"

#include <flecs.h>

#include <raymath.h>

typedef struct {
  v2* vertices;
  z vertex_count;
} c_physics_shape;

ECS_COMPONENT_DECLARE(c_physics_shape);

void c_physics_shape_circle_init(c_physics_shape *shape, f32 radius,
                                 z resolution) {
  shape->vertex_count = resolution;
  shape->vertices = malloc(sizeof(v2) * resolution);

  for (z i = 0; i < resolution; i++) {
    v2 vertex = {.x = radius, .y = 0};
    shape->vertices[i] =
        Vector2Rotate(vertex, 2. * PI * (f32)i / (f32)shape->vertex_count);
  }
}

