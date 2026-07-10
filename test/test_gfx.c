#define PL_USE_GFX
#include "plenum.h"

i32 main(void)
{
    struct pl_gfx_ctx ctx = {
        .backend = PL_GFX_BACKEND_GL,
        .width   = 800,
        .height  = 600,
    };

    if (pl_gfx_init(&ctx) != PL_OK)
    {
        PL_ASSERT_ERRLOG("failed to initialize pl_gfx");
        return PL_EXIT_FAILURE;
    }

    pl_gfx_quit(&ctx);
    return 0;
}
