#pragma once

#include "pl_defs.h"

#define PL_GFX_MAX_WIDTH  12000
#define PL_GFX_MAX_HEIGHT 12000

enum pl_gfx_ctx_state : u32
{
    PL_GFX_STATE_OFF          = 0,
    PL_GFX_STATE_INITIALIZED  = (1 << 0),
    PL_GFX_STATE_BACKEND_INIT = (1 << 1),
    PL_GFX_STATE_RUNNING      = (1 << 2),
};

enum pl_gfx_backend : u32
{
    PL_GFX_BACKEND_NONE = 0,
    PL_GFX_BACKEND_GL,
    PL_GFX_BACKEND_VULKAN,
};

struct pl_gfx_ctx
{
    const char *title;

    void *window;
    void *backend_ctx;

    u32 width, height;

    u32 state;
    u32 backend;
};

pl_status pl_gfx_init(struct pl_gfx_ctx *ctx);
void      pl_gfx_quit(struct pl_gfx_ctx *ctx);
