#pragma once

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;

typedef char i8;
typedef short i16;
typedef int i32;
typedef long i64;

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
