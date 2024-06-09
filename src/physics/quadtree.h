#pragma once

#include "../def.h"

#include <flecs.h>

#include <raylib.h>

#define QUADTREE_ENTITY_CAPACITY 2

typedef struct quadtree_node {
  i64 x, y;
  i64 min, max;

  struct quadtree_node *children[4];

  ecs_entity_t entities[QUADTREE_ENTITY_CAPACITY];
  z entity_cnt;
} quadtree_node;

quadtree_node *root;

void init_root(i64 max) {
  root = malloc(sizeof(quadtree_node));
  memset(root->children, 0, sizeof(quadtree_node *) * 4);

  root->x = root->y = -max / 2;
  root->min = 0;
  root->max = max;
  root->entity_cnt = 0;
}

bool is_in_node(quadtree_node *node, c_transform *transform) {
  i64 x, y;
  c_transform_get_rounded_pos(transform, &x, &y);

  return (x >= node->x + node->min) && (y >= node->y + node->min) &&
         (x <= node->x + node->max) && (y <= node->y + node->max);
}

bool has_children(quadtree_node *node) { return node->children[0] != NULL; }

void add_entity(quadtree_node *node, ecs_entity_t entity, ecs_world_t *world);

void split(quadtree_node *node, ecs_world_t *world) {
  printf("NODE TO SPLIT %d\n", node);
  printf("CHILDREN %d %d %d %d\n", node->children[0], node->children[1],
         node->children[2], node->children[3]);
  EXPECT(node->max / 2 != node->min,
         "Too small quadtree nodes, increase max root size");
  EXPECT(!has_children(node), "Can't split already split tree");

  for (z i = 0; i < 4; i++) {
    node->children[i] = malloc(sizeof(quadtree_node));
    memset(node->children[i]->children, 0, sizeof(quadtree_node *) * 4);

    node->children[i]->x = node->x;
    node->children[i]->y = node->y;

    if (i & 1)
      node->children[i]->x += node->max / 2;
    if (i & 2)
      node->children[i]->y += node->max / 2;

    node->children[i]->min = 0;
    node->children[i]->max = node->max / 2;
    node->children[i]->entity_cnt = 0;
  }

  for (z i = 0; i < node->entity_cnt; i++) {
    c_transform *tranfsorm = ecs_get(world, node->entities[i], c_transform);

    add_entity(node, node->entities[i], world);
  }

  node->entity_cnt = 0;
}

void add_entity(quadtree_node *node, ecs_entity_t entity, ecs_world_t *world) {
  if (has_children(node)) {
    c_transform *tranfsorm = ecs_get(world, entity, c_transform);
    printf("NODE %d COORD %d %d SIZE %d %d ENTITY %f %f\n", node, node->x,
           node->y, node->min, node->max, tranfsorm->position.x,
           tranfsorm->position.y);
    for (z i = 0; i < 4; i++) {
      if (is_in_node(node->children[i], tranfsorm)) {
        printf("INSERT INTO CHILDREN %d ENTITY %f %f\n", node->children[i],
               tranfsorm->position.x, tranfsorm->position.y);
        add_entity(node->children[i], entity, world);
        return;
      }
    }
    PANIC("FUCK, ENTITY OF THIS TREE IS NOT IN THIS TREE???");
    return;
  }

  if (node->entity_cnt == QUADTREE_ENTITY_CAPACITY) {
    split(node, world);
    add_entity(node, entity, world);
  } else
    node->entities[node->entity_cnt++] = entity;
}

void render_quadtree(quadtree_node *node, ecs_world_t *world) {
  DrawRectangleLines(node->x, node->y, node->max, node->max, RAYWHITE);

  for (z i = 0; i < node->entity_cnt; i++) {
    c_transform *tranfsorm = ecs_get(world, node->entities[i], c_transform);
    DrawCircleV(tranfsorm->position, 1.0f, MAGENTA);
  }

  if (has_children(node))
    for (z i = 0; i < 4; i++)
      render_quadtree(node->children[i], world);
}