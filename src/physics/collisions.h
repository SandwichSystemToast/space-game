#pragma once

#include <flecs.h>

#include <float.h>
#include <math.h>

#include "../core/transform.h"
#include "raylib.h"
#include "raymath.h"
#include "shape.h"
#include "src/def.h"

#ifndef MAX_GJK_ITERATIONS
#define MAX_GJK_ITERATIONS 1500
#endif

#ifndef EPA_ERROR_MARGIN
#define EPA_ERROR_MARGIN 0.01
#endif

#ifndef CONCERNING_GJK_ITERATIONS
#define CONCERNING_GJK_ITERATIONS (MAX_GJK_ITERATIONS / 3)
#endif

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
  return Vector2Subtract(
      furthest_point(lhs_shape, lhs_transform, direction),
      furthest_point(rhs_shape, rhs_transform, Vector2Negate(direction)));
}

bool v2_same_direction(v2 a, v2 b) { return Vector2DotProduct(a, b) > 0.; }

// https://github.com/kroitor/gjk.c/blob/master/gjk.c#L27
v2 triple_product(v2 a, v2 b, v2 c) {
  v2 ret;

  f32 ac = a.x * c.x + a.y * c.y;
  f32 bc = b.x * c.x + b.y * c.y;
  ret.x = b.x * ac - a.x * bc;
  ret.y = b.y * ac - a.y * bc;

  return ret;
}

v2 perpendicular(v2 v) {
  v2 ret;
  ret.x = v.y;
  ret.y = -v.x;
  return ret;
}

// Expanding Polytope Algorithm looks for an edge with minimal penetration
// given the simplex, and returns the normal and the penetration depth
v2 epa(c_physics_shape *a_shape, c_transform *a_transform,
       c_physics_shape *b_shape, c_transform *b_transform, v2 simplex[3],
       z simplex_index) {
  z size = (a_shape->vertex_count + b_shape->vertex_count) * sizeof(v2);
  v2 *polytope = malloc(size);
  z polytope_size = simplex_index + 1;
  memset(polytope, 0xCC, size);
  for (z i = 0; i < polytope_size; i++)
    polytope[i] = simplex[i];

  f32 depth = INFINITY;
  v2 normal = Vector2Zero();
  z closest_i2 = 0;

  while (depth == INFINITY) {
    for (z i = 0; i < polytope_size; i++) {
      z i1 = i, i2 = (i + 1) % polytope_size;
      v2 edge = Vector2Subtract(polytope[i1], polytope[i2]);
      v2 candidate_normal = Vector2Normalize(perpendicular(edge));

      f32 candidate_depth = Vector2DotProduct(candidate_normal, polytope[i1]);

      if (candidate_depth < 0.) {
        // Normal is facing away, flip it
        candidate_depth *= -1.;
        candidate_normal = Vector2Negate(candidate_normal);
      }

      if (candidate_depth - depth < EPSILON) {
        depth = candidate_depth;
        normal = candidate_normal;
        closest_i2 = i2;
      }
    }

    v2 next_point =
        support_point(a_shape, a_transform, b_shape, b_transform, normal);

    f32 signed_depth = Vector2DotProduct(normal, next_point);

    if (fabsf(signed_depth - depth) > EPA_ERROR_MARGIN) {
      depth = INFINITY;
      polytope_size++;

      for (z i = polytope_size; i > closest_i2; i--)
        polytope[i] = polytope[i - 1];
      polytope[closest_i2] = next_point;
    }
  }

  return Vector2Scale(normal, (depth + EPA_ERROR_MARGIN));
}

v2 gjk_epa(c_physics_shape *a_shape, c_transform *a_transform,
           c_physics_shape *b_shape, c_transform *b_transform) {
  // TODO: better guess?
  v2 direction = Vector2One();
  direction.y = 0;

  v2 simplex[3];
  z simplex_index = 0;
  v2 a = simplex[0] =
      support_point(a_shape, a_transform, b_shape, b_transform, direction);

  bool collided = false;
  if (Vector2DotProduct(a, direction) <= 0.)
    Vector2Zero();

  direction = Vector2Negate(direction);
  v2 ao, b, c, ab, ac;

  z iterations = 0;
  for (iterations = 1; iterations <= MAX_GJK_ITERATIONS; iterations++) {
    a = simplex[++simplex_index] =
        support_point(a_shape, a_transform, b_shape, b_transform, direction);

    if (Vector2DotProduct(a, direction) <= 0.)
      break;

    ao = Vector2Negate(a);

    if (simplex_index < 2) {
      // line
      b = simplex[0];
      ab = Vector2Subtract(b, a);
      direction = triple_product(ab, ao, ab);
      if (Vector2LengthSqr(direction) == 0)
        direction = perpendicular(ab);
      continue;
    }

    b = simplex[1];
    c = simplex[0];
    ab = Vector2Subtract(b, a);
    ac = Vector2Subtract(c, a);

    if (Vector2DotProduct(triple_product(ab, ac, ac), ao) >= 0) {
      direction = triple_product(ab, ac, ac);
    } else {
      if (Vector2DotProduct(triple_product(ac, ab, ab), ao) < 0.) {
        collided = true;
        break;
      }

      simplex[0] = simplex[1];
      direction = triple_product(ac, ab, ab);
    }

    simplex[1] = simplex[2];
    --simplex_index;
  }

  EXPECT(iterations < CONCERNING_GJK_ITERATIONS,
         "The algorithm took %zu iterations to converge, possibly a bug?",
         iterations);

  if (collided) {
    return epa(a_shape, a_transform, b_shape, b_transform, simplex,
               simplex_index);
  }

  return Vector2Zero();
}

// https://dyn4j.org/2010/04/gjk-gilbert-johnson-keerthi/
void solve_collisions(ecs_iter_t *iterator) {
  ecs_filter_t *f =
      ecs_filter(iterator->world,
                 {.terms = {{ecs_id(c_transform)}, {ecs_id(c_physics_shape)}}});

  ecs_iter_t i_it = ecs_filter_iter(iterator->world, f);
  while (ecs_filter_next(&i_it)) {
    c_transform *i_transform = ecs_field(&i_it, c_transform, 1);
    c_physics_shape *i_shape = ecs_field(&i_it, c_physics_shape, 2);

    ecs_iter_t j_it = ecs_filter_iter(iterator->world, f);
    while (ecs_filter_next(&j_it)) {
      c_transform *j_transform = ecs_field(&j_it, c_transform, 1);
      c_physics_shape *j_shape = ecs_field(&j_it, c_physics_shape, 2);

      for (z i = 0; i < i_it.count; i++) {
        for (z j = 0; j < j_it.count; j++) {
          if (i_it.entities == j_it.entities && i == j)
            continue;

          v2 direction = gjk_epa(&i_shape[i], &i_transform[i], &j_shape[j],
                                 &j_transform[j]);

          DrawLineV(
              Vector2Add(i_transform[i].position, Vector2Negate(direction)),
              i_transform[i].position, GREEN);
        }
      }
    }
  }
}
