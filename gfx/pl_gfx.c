#include "pl_init.h"
#include "pl_io.h"
#include "pl_gfx.h"
#include "pl_gfx_gl.h"

#include <SDL3/SDL.h>

static pl_status init_backend(struct pl_gfx_ctx *ctx);

pl_status pl_gfx_init(struct pl_gfx_ctx *ctx)
{
    if (!pl_is_initialized())
    {
        fprintf(stderr, "Plenum is not initialized");
        return PL_ERROR;
    }

    if (ctx == nullptr)
    {
        return PL_ERROR;
    }

    const char *xdg_session_type = getenv("XDG_SESSION_TYPE");
    const char *wayland_display  = getenv("WAYLAND_DISPLAY");

    if ((xdg_session_type && strcmp(xdg_session_type, "wayland") == 0) ||
        (wayland_display && strlen(wayland_display) > 0))
    {
        SDL_SetHintWithPriority(SDL_HINT_VIDEO_DRIVER, "wayland", SDL_HINT_OVERRIDE);
        pl_log("Detected Wayland, setting SDL_HINT_VIDEO_DRIVER=wayland");
    }
    else
    {
        SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "x11");
        pl_log("Falling back to SDL_HINT_VIDEO_DRIVER=x11");
    }

    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        PL_ASSERT_ERRLOG("failed to initialize SDL3: %s", SDL_GetError());
        return PL_ERROR;
    }

    i32 w = ctx->width;
    i32 h = ctx->height;

    if (w <= 0 || w > PL_GFX_MAX_WIDTH || h <= 0 || h > PL_GFX_MAX_HEIGHT)
    {
        PL_ASSERT_ERRLOG("invalid gfx dimensions: %dx%d", w, h);
        return PL_ERROR;
    }

    const char *name = ctx->title && ctx->title[0] != '\0' ? ctx->title : "plenum";

    SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE;

    switch (ctx->backend)
    {
        case PL_GFX_BACKEND_GL:
        {
            flags |= SDL_WINDOW_OPENGL;
            break;
        }

        case PL_GFX_BACKEND_VULKAN:
        {
            flags |= SDL_WINDOW_VULKAN;
            break;
        }

        case PL_GFX_BACKEND_NONE:
        default:
        {
            break;
        }
    }

    SDL_Window *window = SDL_CreateWindow(name, w, h, flags);

    if (window == nullptr)
    {
        PL_ASSERT_ERRLOG("failed to create SDL_Window: %s", SDL_GetError());
        return PL_ERROR;
    }

    ctx->window = window;

    if (init_backend(ctx) != PL_OK)
    {
        pl_gfx_quit(ctx);
        return PL_ERROR;
    }

    ctx->state |= PL_GFX_STATE_INITIALIZED;
    return PL_OK;
}

void pl_gfx_quit(struct pl_gfx_ctx *ctx)
{
    if (ctx)
    {
        if (ctx->window)
        {
            SDL_DestroyWindow(ctx->window);
        }

        if (ctx->backend_ctx)
        {
            switch (ctx->backend)
            {
                case PL_GFX_BACKEND_GL:
                {
                    pl_gfx_gl_quit(ctx);
                    break;
                }

                case PL_GFX_BACKEND_VULKAN:
                {
                    //
                    break;
                }

                case PL_GFX_BACKEND_NONE:
                default:
                {
                    break;
                }
            }
        }

        SDL_Quit();
    }
}

static pl_status init_backend(struct pl_gfx_ctx *ctx)
{
    switch (ctx->backend)
    {
        case PL_GFX_BACKEND_GL:
        {
            if (pl_gfx_gl_init(ctx) != PL_OK)
            {
                return PL_ERROR;
            }
            break;
        }

        case PL_GFX_BACKEND_VULKAN:
        {
            //
            break;
        }

        case PL_GFX_BACKEND_NONE:
        default:
        {
            PL_ASSERT_ERRLOG("unsupported graphical backend");
            return PL_ERROR;
        }
    }

    return PL_OK;
}
