#pragma once

#include <flecs.h>

#include <float.h>

#include "../core/transform.h"
#include "raylib.h"
#include "raymath.h"
#include "shape.h"

v2 furthest_point(c_physics_shape *shape, c_transform *transform,
                  v2 direction) {
  v2 max_vertex = Vector2Zero();
  f32 max_dot = -FLT_MAX;

  for (z i = 0; i < shape->vertex_count; i++) {
    // TODO: investigate efficiency
    v2 vertex = c_transform_vector(transform, shape->vertices[i]);
    float dot = Vector2DotProduct(vertex, direction);

    if (dot > max_dot) {
      max_dot = dot;
      max_vertex = vertex;
    }
  }

  return max_vertex;
}

v2 support_point(c_physics_shape *lhs_shape, c_transform *lhs_transform,
                 c_physics_shape *rhs_shape, c_transform *rhs_transform,
                 v2 direction) {
  EXPECT(Vector2LengthSqr(direction) != 0,
         "Length of the direction vector can not be zero");

  return Vector2Subtract(
      furthest_point(lhs_shape, lhs_transform, direction),
      furthest_point(rhs_shape, rhs_transform, Vector2Negate(direction)));
}

void solve_collisions(ecs_iter_t *it) {
  c_transform *transform = ecs_field(it, c_transform, 1);
  c_physics_shape *shapes = ecs_field(it, c_physics_shape, 2);

  // XXX: O(N^2), fixme
  for (z i = 0; i < it->count; i++) {
    for (z j = 0; j < it->count; j++) {
      if (i == j)
        continue;

      c_physics_shape *lhs_s = shapes + i, *rhs_s = shapes + j;
      c_transform *lhs_t = transform + i, *rhs_t = transform + j;

      v2 support = support_point(lhs_s, lhs_t, rhs_s, rhs_t, Vector2One());

      DrawCircleV(support, 3., MAGENTA);
    }
  }
}
