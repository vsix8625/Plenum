#pragma once

#ifndef _DEFAULT_SOURCE
    #define _DEFAULT_SOURCE
#endif

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

typedef __int128_t  i128;
typedef __uint128_t u128;

typedef float  f32;
typedef double f64;

typedef u8        byte;
typedef uintptr_t uptr;
typedef intptr_t  iptr;

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

//----------------------------------------------------------------------------------------------------
// Limits

#define PL_BUF_SIZE_16   (1 << 4)
#define PL_BUF_SIZE_32   (1 << 5)
#define PL_BUF_SIZE_64   (1 << 6)
#define PL_BUF_SIZE_128  (1 << 7)
#define PL_BUF_SIZE_256  (1 << 8)
#define PL_BUF_SIZE_512  (1 << 9)
#define PL_BUF_SIZE_1024 (1 << 10)
#define PL_BUF_SIZE_2048 (1 << 11)
#define PL_BUF_SIZE_4096 (1 << 12)
#define PL_BUF_SIZE_8192 (1 << 13)
#define PL_BUF_SIZE_16K  (1 << 14)
#define PL_BUF_SIZE_32K  (1 << 15)
#define PL_BUF_SIZE_64K  (1 << 16)

#define PL_PATH_MAX 4096

#define PL_U64_MAX (UINT64_MAX)

//----------------------------------------------------------------------------------------------------

#ifndef pl_malloc
    #define pl_malloc malloc
#endif

#ifndef pl_calloc
    #define pl_calloc calloc
#endif

#ifndef pl_realloc
    #define pl_realloc realloc
#endif

#ifndef pl_strdup
    #define pl_strdup strdup
#endif

#ifndef pl_free
    #define pl_free free
#endif
