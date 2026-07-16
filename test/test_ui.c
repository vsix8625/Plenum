#include "plenum.h"

i32 main(void)
{
    pl_io_set_prefix(PL_IO_LOG_LEVEL_LOG, "[test_ui]: ", PL_IO_COLOR_GREEN);

    struct pl_sdl3_ctx ctx1 = {
        .backend = PL_SDL_BACKEND_GL,
        .width   = 800,
        .height  = 600,
        .title   = "window 1",
    };
    if (pl_sdl3_video_init(&ctx1) != PL_OK)
    {
        PL_ASSERT_ERRLOG("failed to initialize ctx1");
        return PL_EXIT_FAILURE;
    }
    pl_log("Initialized: ctx1");

    struct pl_sdl3_ctx ctx2 = {
        .backend = PL_SDL_BACKEND_GL,
        .width   = 640,
        .height  = 480,
        .title   = "window 2",
    };
    if (pl_sdl3_video_init(&ctx2) != PL_OK)
    {
        PL_ASSERT_ERRLOG("failed to initialize ctx2");
        pl_sdl3_video_quit(&ctx1);
        return PL_EXIT_FAILURE;
    }
    pl_log("Initialized: ctx2");

    struct pl_sdl3_ctx *ctx3 = pl_arena_push(pl_get_global_arena(), sizeof(struct pl_sdl3_ctx));
    if (ctx3)
    {
        ctx3->backend = PL_SDL_BACKEND_GL;
        ctx3->width   = 450;
        ctx3->height  = 850;
        ctx3->title   = "window 3";
        if (pl_sdl3_video_init(ctx3) != PL_OK)
        {
            PL_ASSERT_ERRLOG("failed to initialize ctx3");
            pl_sdl3_quit_all();
            return PL_EXIT_FAILURE;
        }
        pl_log("Initialized: ctx3");
    }

    struct pl_sdl3_audio_ctx audio_ctx = {0};

    if (pl_sdl3_audio_init(&audio_ctx) != PL_OK)
    {
        PL_ASSERT_ERRLOG("failed to initialize audio_ctx");
        pl_sdl3_quit_all();
        return PL_EXIT_FAILURE;
    }
    pl_log("Initialized: audio_ctx");

    // shutdown
    pl_sdl3_video_quit(&ctx1);
    pl_log("Quit: ctx1");
    pl_sdl3_video_quit(&ctx2);
    pl_log("Quit: ctx2");
    pl_sdl3_video_quit(ctx3);
    pl_log("Quit: ctx3");

    if (pl_sdl3_is_init())
    {
        pl_sdl3_quit_all();
    }

    return 0;
}
