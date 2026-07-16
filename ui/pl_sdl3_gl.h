#pragma once

#include "pl_defs.h"

struct pl_sdl3_ctx;
typedef struct SDL_FColor SDL_FColor;

pl_status pl_sdl3_gl_init(struct pl_sdl3_ctx *ctx);
void      pl_sdl3_gl_quit(struct pl_sdl3_ctx *ctx);
void      pl_sdl3_gl_begin(struct pl_sdl3_ctx *ctx, SDL_FColor fcolor);
void      pl_sdl3_gl_end(struct pl_sdl3_ctx *ctx);
