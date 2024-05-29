# Philosophy

This document contains design decisions that are arbitrary conventions in the code.

## Naming

All components are prefixed with `c_`. Examples: `c_transform`, `c_camera`, `c_player_character`.

## Winding Order

All polygons are wound counterclockwise (CCW). The holes in said polygons should be wound clockwise (CW).
