#pragma once

#include <math.h>
#define _XOPEN_SOURCE 700
#define _POSIX_C_SOURCE 200809L

#include <raylib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float_t f32;

typedef size_t z;

typedef Vector2 v2;
typedef Vector3 v3;
typedef Vector4 v4;

#define PANIC(msg, ...)                                                        \
  {                                                                            \
    fprintf(stderr, __FILE__ ":%d\n" msg "\n", __LINE__, ##__VA_ARGS__);       \
    exit(2);                                                                   \
  }

#define EXPECT(expr, msg, ...)                                                 \
  {                                                                            \
    if (!(expr)) {                                                             \
      PANIC(msg, ##__VA_ARGS__)                                                \
    }                                                                          \
  }
