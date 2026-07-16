#pragma once

#include "pl_defs.h"
#include <SDL3/SDL_audio.h>

#define PL_SDL_WINDOW_MAX_WIDTH  12000
#define PL_SDL_WINDOW_MAX_HEIGHT 12000

enum pl_sdl3_ctx_state : u32
{
    PL_SDL_STATE_OFF          = 0,
    PL_SDL_STATE_INITIALIZED  = (1 << 0),
    PL_SDL_STATE_BACKEND_INIT = (1 << 1),
    PL_SDL_STATE_RUNNING      = (1 << 2),
};

enum pl_sdl3_backend : u32
{
    PL_SDL_BACKEND_NONE = 0,
    PL_SDL_BACKEND_GL,
    PL_SDL_BACKEND_VULKAN,
};

struct pl_sdl3_ctx
{
    const char *title;

    void *window;
    void *backend_ctx;

    u32 width, height;

    u32 state;
    u32 backend;
};

enum pl_sdl3_audio_state : u32
{
    PL_SDL_AUDIO_STATE_OFF         = 0,
    PL_SDL_AUDIO_STATE_INITIALIZED = (1 << 0),
};

struct pl_sdl3_audio_ctx
{
    SDL_AudioDeviceID device_id;
    SDL_AudioFormat   format;

    void *user_data;

    u32 sample_rate;
    u32 state;
    u16 channels;
};

/* Checks if the graphics or audio subsystems are active. */
bool pl_sdl3_is_init(void);

/* Completely shut down all active `SDL3` subsystems. */
void pl_sdl3_quit_all(void);

/* Initializes an independent window context with the selected graphics backend.
 * Increments `SDL3` video subsystem reference counter. */
pl_status pl_sdl3_video_init(struct pl_sdl3_ctx *ctx);

/* Cleans up a specific window and its backend resources.
 * Decrements `SDL3` video subsystem reference counter. */
void pl_sdl3_video_quit(struct pl_sdl3_ctx *ctx);

/* Initializes an independent audio device playback stream.
 * Increments `SDL3` audio subsystem reference counter. */
pl_status pl_sdl3_audio_init(struct pl_sdl3_audio_ctx *ctx);

/* Closes a specific audio device stream.
 * Decrements `SDL3` audio subsystem reference counter. */
void pl_sdl3_audio_quit(struct pl_sdl3_audio_ctx *ctx);
