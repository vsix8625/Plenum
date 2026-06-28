#include "pl_arena.h"
#include "pl_init.h"

pl_status pl_arena_init(void)
{
    if (!pl_is_initialized())
    {
        return PL_FATAL;
    }
    return PL_OK;
}
