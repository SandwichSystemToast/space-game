#pragma once

#include "../def.h"

#include <float.h>

#include <flecs.h>

#include <raymath.h>

typedef struct {
  v2 *vertices;
  z vertex_count;
} c_physics_shape;

ECS_COMPONENT_DECLARE(c_physics_shape);

bool check_vertex_winding(v2 *vertices, z vertex_count) {
  for (int i = 0; i < vertex_count; i++)
    if (Vector2DotProduct(vertices[i], vertices[(i + 1) % vertex_count]) < 0.)
      return false;
  return true;
}

v2 c_physics_shape_naive_center(c_physics_shape *shape) {
  v2 center = Vector2Zero();
  for (z i = 0; i < shape->vertex_count; i++)
    center = Vector2Add(
        center, Vector2Scale(shape->vertices[i], 1 / (f32)shape->vertex_count));
  return center;
}

v2 c_physics_shape_furtest_point(c_physics_shape *shape, v2 direction) {
  v2 max_vertex = Vector2Zero();
  f32 max_dot = -INFINITY;

  for (z i = 0; i < shape->vertex_count; i++) {
    v2 vertex = shape->vertices[i];
    float dot = Vector2DotProduct(vertex, direction);

    if (dot > max_dot) {
      max_dot = dot;
      max_vertex = vertex;
    }
  }

  return max_vertex;
}

void c_physics_shape_circle_init(c_physics_shape *shape, f32 radius,
                                 z resolution) {
  shape->vertex_count = resolution;
  shape->vertices = malloc(sizeof(v2) * resolution);

  for (z i = 0; i < resolution; i++) {
    v2 vertex = {.x = radius, .y = 0};
    f32 angle = 2. * PI * (f32)i / (f32)shape->vertex_count;
    shape->vertices[i] = Vector2Rotate(vertex, angle);
  }

  EXPECT(check_vertex_winding(shape->vertices, shape->vertex_count),
         "Expected counter-clockwise winding for a circle shape")
}
