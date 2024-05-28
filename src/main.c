#include "def.h"

#include "raylib.h"
#include "raymath.h"

int main(void) {
  InitWindow(800, 450, "Space Game");

  // https://github.com/id-Software/Quake/blob/master/WinQuake/sv_user.c#L190
  v2 position = {0, 0};
  v2 velocity = {0, 0};
  f32 max_speed = 240.;
  f32 max_acceleration = 10. * max_speed;
  f32 drag = 10.;

  while (!WindowShouldClose()) {
    Camera2D cam;
    cam.rotation = 0.;
    cam.zoom = 1.;
    cam.offset.x = GetScreenWidth() / 2.;
    cam.offset.y = GetScreenHeight() / 2.;

    BeginMode2D(cam);
    ClearBackground(BLACK);

    f32 dt = GetFrameTime();
    v2 direction = {0};
    if (IsKeyDown(KEY_D))
      direction.x += 1.;
    if (IsKeyDown(KEY_A))
      direction.x -= 1;
    if (IsKeyDown(KEY_W))
      direction.y -= 1;
    if (IsKeyDown(KEY_S))
      direction.y += 1;
    direction = Vector2Normalize(direction);
    velocity = Vector2Scale(velocity, (1 - drag * dt));

    f32 current_speed = Vector2DotProduct(velocity, direction);
    if (current_speed > max_speed)
      current_speed = max_speed;
    f32 acceleration = max_speed - current_speed;
    if (acceleration < 0.)
      acceleration = 0.;
    else if (acceleration > max_acceleration * dt)
      acceleration = max_acceleration * dt;

    velocity = Vector2Add(velocity, Vector2Scale(direction, acceleration));
    position = Vector2Add(position, Vector2Scale(velocity, dt));

    DrawLineV(
        position,
        Vector2Add(position, Vector2Scale(direction, 40.)),
        BLUE);
    DrawLineV(position, Vector2Add(position, velocity), RED);

    DrawCircleV(position, 15., WHITE);
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
