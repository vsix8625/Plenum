#include "pl_init.h"
#include "pl_io.h"
#include "pl_sdl3.h"
#include "pl_sdl3_gl.h"

#include <SDL3/SDL.h>

static pl_status init_backend(struct pl_sdl3_ctx *ctx);

void pl_sdl3_setup(void)
{
    const char *xdg_session_type = getenv("XDG_SESSION_TYPE");
    const char *wayland_display  = getenv("WAYLAND_DISPLAY");

    if ((xdg_session_type && strcmp(xdg_session_type, "wayland") == 0) ||
        (wayland_display && strlen(wayland_display) > 0))
    {
        SDL_SetHintWithPriority(SDL_HINT_VIDEO_DRIVER, "wayland", SDL_HINT_OVERRIDE);
        pl_dbg("Detected Wayland, setting SDL_HINT_VIDEO_DRIVER=wayland");
    }
    else
    {
        SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "x11");
        pl_dbg("Falling back to SDL_HINT_VIDEO_DRIVER=x11");
    }
    pl_log("%s: ok", __func__);
}

bool pl_sdl3_is_init(void)
{
    u32 flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO;
    if (SDL_WasInit(flags) & flags)
    {
        return true;
    }
    return false;
}

void pl_sdl3_quit_all(void)
{
    u32 flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO;
    if (SDL_WasInit(flags) & flags)
    {
        SDL_Quit();
        pl_dbg("%s: Cleanup SDL3 systems", __func__);
    }
}

pl_status pl_sdl3_video_init(struct pl_sdl3_ctx *ctx)
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

    if (!SDL_InitSubSystem(SDL_INIT_VIDEO))
    {
        PL_ASSERT_ERRLOG("failed to initialize SDL3: %s", SDL_GetError());
        return PL_ERROR;
    }
    pl_dbg("%s: SDL_InitSubSystem(SDL_INIT_VIDEO): ok", __func__);

    i32 w = ctx->width;
    i32 h = ctx->height;

    if (w <= 0 || w > PL_SDL_WINDOW_MAX_WIDTH || h <= 0 || h > PL_SDL_WINDOW_MAX_HEIGHT)
    {
        PL_ASSERT_ERRLOG("invalid window dimensions: %dx%d", w, h);
        return PL_ERROR;
    }

    const char *name = ctx->title && ctx->title[0] != '\0' ? ctx->title : "plenum";

    SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE;

    switch (ctx->backend)
    {
        case PL_SDL_BACKEND_GL:
        {
            flags |= SDL_WINDOW_OPENGL;
            break;
        }

        case PL_SDL_BACKEND_VULKAN:
        {
            flags |= SDL_WINDOW_VULKAN;
            break;
        }

        case PL_SDL_BACKEND_NONE:
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
    pl_dbg("%s: SDL_CreateWindow: ok | Res: %dx%d", __func__, w, h);

    ctx->window = window;

    if (init_backend(ctx) != PL_OK)
    {
        pl_sdl3_video_quit(ctx);
        return PL_ERROR;
    }

    ctx->state |= PL_SDL_STATE_INITIALIZED;
    pl_dbg("%s: ok", __func__);
    return PL_OK;
}

void pl_sdl3_video_quit(struct pl_sdl3_ctx *ctx)
{
    if (ctx)
    {
        if (ctx->window)
        {
            SDL_DestroyWindow(ctx->window);
            ctx->window = nullptr;
        }

        if (ctx->backend_ctx)
        {
            switch (ctx->backend)
            {
                case PL_SDL_BACKEND_GL:
                {
                    pl_sdl3_gl_quit(ctx);
                    break;
                }

                case PL_SDL_BACKEND_VULKAN:
                {
                    //
                    break;
                }

                case PL_SDL_BACKEND_NONE:
                default:
                {
                    break;
                }
            }

            ctx->backend_ctx = nullptr;
        }

        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        pl_dbg("%s: SDL_QuitSubSystem(SDL_INIT_VIDEO): ok", __func__);
    }
}

static pl_status init_backend(struct pl_sdl3_ctx *ctx)
{
    switch (ctx->backend)
    {
        case PL_SDL_BACKEND_GL:
        {
            if (pl_sdl3_gl_init(ctx) != PL_OK)
            {
                return PL_ERROR;
            }
            break;
        }

        case PL_SDL_BACKEND_VULKAN:
        {
            //
            break;
        }

        case PL_SDL_BACKEND_NONE:
        default:
        {
            PL_ASSERT_ERRLOG("unsupported graphical backend");
            return PL_ERROR;
        }
    }

    pl_dbg("%s: ok", __func__);
    return PL_OK;
}

pl_status pl_sdl3_audio_init(struct pl_sdl3_audio_ctx *ctx)
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

    if (!SDL_InitSubSystem(SDL_INIT_AUDIO))
    {
        PL_ASSERT_ERRLOG("failed to initialize SDL3 Audio: %s", SDL_GetError());
        return PL_ERROR;
    }
    pl_dbg("%s: SDL_InitSubSystem(SDL_INIT_AUDIO): ok", __func__);

    // defaults
    SDL_AudioSpec spec = {.format   = ctx->format != 0 ? ctx->format : SDL_AUDIO_S16,
                          .channels = ctx->channels != 0 ? ctx->channels : 2,
                          .freq     = ctx->sample_rate != 0 ? ctx->sample_rate : 44100};

    SDL_AudioDeviceID id = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
    if (id == 0)
    {
        PL_ASSERT_ERRLOG("failed to open audio device: %s", SDL_GetError());
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return PL_ERROR;
    }

    ctx->device_id  = id;
    ctx->state     |= PL_SDL_AUDIO_STATE_INITIALIZED;

    pl_dbg("%s: Audio device opened | ID: %u | %dHz | %d channels",
           __func__,
           id,
           spec.freq,
           spec.channels);

    return PL_OK;
}

void pl_sdl3_audio_quit(struct pl_sdl3_audio_ctx *ctx)
{
    if (ctx)
    {
        if (ctx->device_id != 0)
        {
            SDL_CloseAudioDevice(ctx->device_id);
            ctx->device_id = 0;
        }

        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        pl_dbg("%s: SDL_QuitSubSystem(SDL_INIT_AUDIO): ok", __func__);

        ctx->state &= ~PL_SDL_AUDIO_STATE_INITIALIZED;
    }
}
