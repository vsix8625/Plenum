#include "pl_init.h"
#include "pl_arena.h"
#include "pl_io.h"
#include "pl_internal.h"

#include <stdatomic.h>

static atomic_int g_pl_is_initialized = 0;

struct pl_arena *g_pl_global_arena = nullptr;

pl_status pl_init(void)
{
    if (atomic_load(&g_pl_is_initialized))
    {
        // already initialized
        return PL_ERROR;
    }
    atomic_store(&g_pl_is_initialized, 1);

    if (pl_arena_init() != PL_OK)
    {
        PL_ASSERT_ERRLOG("failed to initialized arena system");
        return PL_FATAL;
    }

    g_pl_global_arena = pl_arena_create("pl_global_arena", PL_MiB(2));

    if (g_pl_global_arena == nullptr)
    {
        PL_ASSERT_ERRLOG("failed to create global arena");
        return PL_FATAL;
    }
    return PL_OK;
}

void pl_quit(void)
{
    if (g_pl_global_arena)
    {
        pl_arena_destroy(g_pl_global_arena);
    }

    pl_arena_quit();
    atomic_store(&g_pl_is_initialized, 0);
}

bool pl_is_initialized(void)
{
    return atomic_load(&g_pl_is_initialized);
}
