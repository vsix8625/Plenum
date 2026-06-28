#include "pl_init.h"
#include <stdatomic.h>

static atomic_int g_pl_is_initialized = 0;

pl_status pl_init(void)
{
    if (atomic_load(&g_pl_is_initialized))
    {
        // already initialized
        return PL_ERROR;
    }

    atomic_store(&g_pl_is_initialized, 1);
    return PL_OK;
}

void pl_quit(void)
{
    atomic_store(&g_pl_is_initialized, 0);
}

bool pl_is_initialized(void)
{
    return atomic_load(&g_pl_is_initialized);
}
