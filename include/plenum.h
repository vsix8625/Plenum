#pragma once

#include "pl_defs.h"    // IWYU pragma: export
#include "pl_init.h"    // IWYU pragma: export
#include "pl_arena.h"   // IWYU pragma: export
#include "pl_io.h"      // IWYU pragma: export
#include "pl_util.h"    // IWYU pragma: export
#include "pl_fs.h"      // IWYU pragma: export
#include "pl_string.h"  // IWYU pragma: export
#include "pl_cpu.h"     // IWYU pragma: export
#include "pl_sys.h"     // IWYU pragma: export
#include "pl_time.h"    // IWYU pragma: export
#include "pl_array.h"   // IWYU pragma: export
#include "pl_thread.h"  // IWYU pragma: export

#if defined(PL_USE_UI)
    #include "pl_sdl3.h"  // IWYU pragma: export
#endif
