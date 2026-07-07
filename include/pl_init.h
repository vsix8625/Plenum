#pragma once

#include "pl_defs.h"  // IWYU pragma: export

#if defined(PL_USE_CONSTRUCTOR)
    #if defined(__GNUC__) || defined(__clang__)
        #define PL_CTOR __attribute__((constructor))
        #define PL_DTOR __attribute__((destructor))
    #else
        #error "Constructors are not supported on this compiler. Remove PL_USE_CONSTRUCTOR"
    #endif
#else
    #define PL_CTOR
    #define PL_DTOR
#endif

#if !defined(PL_USE_CONSTRUCTOR)

pl_status pl_init(void);
void      pl_quit(void);

#endif

bool             pl_is_initialized(void);
struct pl_arena *pl_get_global_arena(void);
