#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float  f32;
typedef double f64;

typedef enum pl_exit : i32
{
    PL_EXIT_SUCCESS = 0,
    PL_EXIT_FAILURE = 1,
} pl_exit;

typedef enum pl_status : i32
{
    PL_NOT_INITIALIZED = -3,
    PL_ERROR           = -2,
    PL_FATAL           = -1,
    PL_OK              = 0,
} pl_status;

#if defined(__GNUC__) || defined(__clang__)
    #define PL_INTERNAL __attribute__((visibility("hidden")))
#elif defined(_MSC_VER)
    #define PL_INTERNAL
#else
    #define PL_INTERNAL
#endif

#if !defined(__cplusplus)
    #if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 202311L
        #ifndef nullptr
            #define nullptr ((void *) 0)
        #endif
    #endif
#endif

static_assert(sizeof(void *) == 8);  // std=c23

#define PL_KiB(x) ((u64) (x) << 10)
#define PL_MiB(x) ((u64) (x) << 20)
#define PL_GiB(x) ((u64) (x) << 30)
