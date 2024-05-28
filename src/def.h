#pragma once

#define _XOPEN_SOURCE 700
#define _POSIX_C_SOURCE 200809L

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef size_t z;

#define PANIC(msg, ...)                                                        \
  {                                                                            \
    fprintf(stderr, __FILE__ ":%d\n" msg "\n", __LINE__, ##__VA_ARGS__);            \
    exit(2);                                                                   \
  }

#define EXPECT(expr, msg, ...)                                                 \
  {                                                                            \
    if (!(expr)) {                                                             \
      PANIC(msg, ##__VA_ARGS__)                                                \
    }                                                                          \
  }
