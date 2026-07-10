#pragma once

#include "pl_defs.h"

struct pl_gfx_ctx;

pl_status pl_gfx_gl_init(struct pl_gfx_ctx *ctx);
void      pl_gfx_gl_quit(struct pl_gfx_ctx *ctx);
