# FAQ

Well, I knew this before I started debugging.

## `INVALID_PARAMETER`

Are your components registered?

## `ecs_iter_t->count` does not match

The systems run once per each archetype, not all instances of the components at the same time. Consider using [`ecs_filter_t`](https://www.flecs.dev/flecs/md_docs_2Queries.html#filters) or [`ecs_query_t`](https://www.flecs.dev/flecs/md_docs_2Queries.html#cached-queries).

## `ecs_id(T) == 0`

The component is *declared* but not *defined*. Use `ECS_COMPONENT_DEFINE` and `ECS_COMPONENT_DECLARE`;

## `FLECS_ID##T##_`

The component is not *declared* or *defined*. Remember to let flecs know about your components.
