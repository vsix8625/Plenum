#pragma once

#include "pl_defs.h"

u64 pl_cpu_nproc(void);
u64 pl_cpu_cacheline(void);

#if defined(__x86_64__) || defined(_M_X64)
    #define PL_ARCH_NAME "x86_64"
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define PL_ARCH_NAME "arm64"
#elif defined(__i386__) || defined(_M_IX86)
    #define PL_ARCH_NAME "x86"
#elif defined(__arm__)
    #define PL_ARCH_NAME "arm"
#else
    #define PL_ARCH_NAME "unknown"
#endif

static inline bool pl_cpu_has_avx(void)
{
    return __builtin_cpu_supports("avx");
}

static inline bool pl_cpu_has_avx2(void)
{
    return __builtin_cpu_supports("avx2");
}

static inline bool pl_cpu_has_sse4_2(void)
{
    return __builtin_cpu_supports("sse4.2");
}

static inline bool pl_cpu_has_bmi(void)
{
    return __builtin_cpu_supports("bmi");
}
