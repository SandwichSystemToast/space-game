#pragma once

#include "../def.h"

#include <flecs.h>

#include <raylib.h>

#define QUADTREE_ENTITY_CAPACITY 2

typedef struct quadtree_node {
  i64 x, y;
  i64 size;

  struct quadtree_node *children[4];

  ecs_entity_t entities[QUADTREE_ENTITY_CAPACITY];
  z entity_cnt;
} quadtree_node;

quadtree_node *root;

void init_root(i64 size) {
  root = malloc(sizeof(quadtree_node));
  memset(root->children, 0, sizeof(quadtree_node *) * 4);

  root->x = root->y = 0;
  root->size = size;
  root->entity_cnt = 0;
}

bool is_in_node(quadtree_node *node, c_transform *transform) {
  i64 x, y;
  c_transform_get_rounded_pos(transform, &x, &y);
  printf("BOX %ld %ld %ld ENTITY BOUND CHECK %ld %ld\n", node->x, node->y,
         node->size, x, y);

  return (x > node->x - node->size / 2) && (y > node->y - node->size / 2) &&
         (x <= node->x + node->size / 2) && (y <= node->y + node->size / 2);
}

bool has_children(quadtree_node *node) { return node->children[0] != NULL; }

void add_entity(quadtree_node *node, ecs_entity_t entity, ecs_world_t *world);

void split(quadtree_node *node, ecs_world_t *world) {
  printf("NODE TO SPLIT %d\n", node);
  printf("CHILDREN %d %d %d %d\n", node->children[0], node->children[1],
         node->children[2], node->children[3]);
  EXPECT(node->size / 4 != 0,
         "Too small quadtree nodes, increase max root size");
  EXPECT(!has_children(node), "Can't split already split tree");

  for (z i = 0; i < 4; i++) {
    node->children[i] = malloc(sizeof(quadtree_node));
    memset(node->children[i]->children, 0, sizeof(quadtree_node *) * 4);

    node->children[i]->x = node->x;
    node->children[i]->y = node->y;

    if (i & 1)
      node->children[i]->x += node->size / 4;
    else
      node->children[i]->x -= node->size / 4;

    if (i & 2)
      node->children[i]->y += node->size / 4;
    else
      node->children[i]->y -= node->size / 4;

    node->children[i]->size = node->size / 2;
    node->children[i]->entity_cnt = 0;
  }

  for (z i = 0; i < node->entity_cnt; i++) {
    c_transform *tranfsorm = ecs_get(world, node->entities[i], c_transform);
    tranfsorm->local_position.x += node->x;
    tranfsorm->local_position.y += node->y;

    add_entity(node, node->entities[i], world);
  }

  node->entity_cnt = 0;
}

void add_entity(quadtree_node *node, ecs_entity_t entity, ecs_world_t *world) {
  c_transform *tranfsorm = ecs_get(world, entity, c_transform);

  if (has_children(node)) {
    printf("NODE %d COORD %d %d SIZE %d ENTITY %f %f\n", node, node->x, node->y,
           node->size, tranfsorm->local_position.x,
           tranfsorm->local_position.y);
    for (z i = 0; i < 4; i++) {
      if (is_in_node(node->children[i], tranfsorm)) {
        printf("INSERT INTO CHILDREN %d ENTITY %f %f\n", node->children[i],
               tranfsorm->local_position.x, tranfsorm->local_position.y);
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
  } else {
    tranfsorm->local_position.x -= node->x;
    tranfsorm->local_position.y -= node->y;
    node->entities[node->entity_cnt++] = entity;
  }
}

void render_quadtree(quadtree_node *node, ecs_world_t *world) {
  DrawRectangleLines(node->x - node->size / 2, node->y - node->size / 2,
                     node->size, node->size, RAYWHITE);

  for (z i = 0; i < node->entity_cnt; i++) {
    c_transform *tranfsorm = ecs_get(world, node->entities[i], c_transform);
    DrawCircle(tranfsorm->local_position.x + node->x,
               tranfsorm->local_position.y + node->y, 1.0f, MAGENTA);
  }

  if (has_children(node))
    for (z i = 0; i < 4; i++)
      render_quadtree(node->children[i], world);
}