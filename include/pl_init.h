#pragma once

#include "pl_defs.h"

extern struct pl_arena *g_pl_global_arena;

pl_status pl_init(void);
void      pl_quit(void);
bool      pl_is_initialized(void);
