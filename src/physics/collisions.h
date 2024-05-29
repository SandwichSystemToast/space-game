#pragma once

#include <flecs.h>

#include <float.h>

#include "../core/transform.h"
#include "raylib.h"
#include "raymath.h"
#include "shape.h"
#include "src/def.h"

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

          // TODO: better guess?
          v2 direction = Vector2One();
          direction.y = 0;

          v2 simplex[3];
          z simplex_index = 0;
          v2 a = simplex[0] =
              support_point(&i_shape[i], &i_transform[i], &j_shape[j],
                            &j_transform[j], direction);

          bool collided = false;
          if (Vector2DotProduct(a, direction) <= 0.) {
            goto done;
          }

          direction = Vector2Negate(direction);
          v2 ao, b, c, ab, ac;
          for (z iter = 0; iter < 1500; iter++) {

            a = simplex[++simplex_index] =
                support_point(&i_shape[i], &i_transform[i], &j_shape[j],
                              &j_transform[j], direction);

            if (Vector2DotProduct(a, direction) <= 0.) {
              goto done;
            }

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
                goto done;
              }

              simplex[0] = simplex[1];
              direction = triple_product(ac, ab, ab);
            }

            simplex[1] = simplex[2];
            --simplex_index;
          }

        done:
          if (collided) {
            DrawCircleV(Vector2Zero(), 10., GREEN);
          }
        }
      }
    }
  }

  /*

  for (int i = 0; i < it.count; i++) {
      c_transform *transform = ecs_field(&it, c_transform, 1);
      c_physics_shape *shape = ecs_field(&it, c_physics_shape, 2);

      for (z j = 0; j < shape->vertex_count; j++) {
        z j1 = j;
        z j2 = (j + 1) % shape->vertex_count;

        DrawLineV(Vector2Add(transform->position, shape->vertices[j1]),
                  Vector2Add(transform->position, shape->vertices[j2]), RED);
      }
    }

  */
}

/*
          for (;;) {
            support = support_point(&i_shape[i], &i_transform[i], &j_shape[j],
                                    &j_transform[j], direction);

            if (Vector2DotProduct(support, direction) <= 0.) {
              has_collided = false;
              break;
            }

            simplex[2] = simplex[1];
            simplex[1] = simplex[0];
            simplex[0] = support;
            simplex_points++;

            if (simplex_points == 2) {
              v2 a = simplex[0], b = simplex[1];
              v2 ab = Vector2Subtract(b, a);
              v2 ao = Vector2Negate(a);

              if (Vector2DotProduct(ab, ao) > 0) {
                direction = Vector2Negate(direction);
              } else {
                simplex[0] = a;
                direction = ao;
                simplex_points = 1;
              }
            } else if (simplex_points == 3) {
            }
          }

          if (has_collided) {
            DrawCircleV(Vector2Zero(), 6., GREEN);
          }

          DrawLineV(simplex[0], simplex[1], RED);
          DrawLineV(simplex[1], simplex[2], RED);
          DrawLineV(simplex[2], simplex[0], RED);

          // TODO: PHYSICS

*/

/*
void solve_collisions(ecs_iter_t *iterator) {
  ecs_filter_t *f =
      ecs_filter(iterator->world,
                 {.terms = {{ecs_id(c_transform)}, {ecs_id(c_physics_shape)}}});
  ecs_iter_t it = ecs_filter_iter(iterator->world, f);

  while (ecs_filter_next(&it)) {
    c_transform *transforms = ecs_field(&it, c_transform, 1);
    c_physics_shape *shapes = ecs_field(&it, c_physics_shape, 2);

    // XXX: O(N^2), fixme
    for (z i = 0; i < it.count; i++) {
      for (z j = 0; j < it.count; j++) {
        if (i == j)
          continue;

        c_physics_shape *lhs_s = shapes + i, *rhs_s = shapes + j;
        c_transform *lhs_t = transforms + i, *rhs_t = transforms + j;

        v2 support = support_point(lhs_s, lhs_t, rhs_s, rhs_t, Vector2One());

        DrawCircleV(support, 3., MAGENTA);
      }
    }
  }
}
*/
