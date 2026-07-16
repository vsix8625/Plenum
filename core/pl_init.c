#include "pl_init.h"
#include "pl_arena.h"
#include "pl_io.h"
#include "pl_internal.h"

#include <stdatomic.h>

static atomic_int g_pl_is_initialized = 0;

static struct pl_arena *g_pl_global_arena = nullptr;

struct pl_arena *pl_get_global_arena(void)
{
    if (!g_pl_is_initialized)
    {
        return nullptr;
    }

    return g_pl_global_arena;
}

static pl_status init_stage_two(void)
{
#if defined(DEBUG)
    pl_io_set_debug(true);
#elif defined(DNDEBUG)
    pl_io_set_debug(false);
#endif

    if (pl_arena_init() != PL_OK)
    {
        PL_ASSERT_ERRLOG("failed to initialized arena system");
        atomic_store(&g_pl_is_initialized, 0);
        return PL_FATAL;
    }
    pl_dbg("pl_arena_init: ok");

    g_pl_global_arena = pl_arena_create("pl_global_arena", PL_MiB(2));

    if (g_pl_global_arena == nullptr)
    {
        PL_ASSERT_ERRLOG("failed to create global arena");
        atomic_store(&g_pl_is_initialized, 0);
        return PL_FATAL;
    }
    pl_dbg("pl_arena_create: ok | Size: %lu bytes", g_pl_global_arena->capacity);

    pl_io_set_prefix(PL_IO_LOG_LEVEL_LOG, "[plenum]: ", PL_IO_COLOR_GREEN);

    // pl_time
    pl_time_init();
    pl_dbg("pl_time_init: ok");

#if defined(PL_USE_UI)
    pl_sdl3_setup();
#endif

    pl_dbg("%s: ok", __func__);
    return PL_OK;
}

PL_CTOR pl_status pl_init(void)
{
    if (atomic_load(&g_pl_is_initialized))
    {
        // already initialized
        return PL_ERROR;
    }
    atomic_store(&g_pl_is_initialized, 1);

    pl_dbg("%s: ok", __func__);
    return init_stage_two();
}

PL_DTOR void pl_quit(void)
{
    if (g_pl_global_arena)
    {
        pl_arena_destroy(g_pl_global_arena);
    }
    pl_dbg("%s: pl_arena_destroy: ok", __func__);

    pl_arena_quit();
    pl_dbg("%s: pl_arena_quit: ok", __func__);
    pl_dbg("%s: ok", __func__);
    atomic_store(&g_pl_is_initialized, 0);
}

bool pl_is_initialized(void)
{
    return atomic_load(&g_pl_is_initialized);
}
