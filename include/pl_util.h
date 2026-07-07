#pragma once

#include "pl_defs.h"

static inline void pl_util_pause(void)
{
#if defined(__x86_64__) || defined(__i386__)
    __builtin_ia32_pause();
#else
    __asm__ __volatile__("" ::: "memory");
#endif
}

void  pl_util_yield(void);
char *pl_trimstr(char *s);
