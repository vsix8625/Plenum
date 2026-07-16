#include "pl_sdl3.h"
#include "pl_sdl3_gl.h"
#include "pl_io.h"
#include "glad.h"

#include <SDL3/SDL_video.h>
#include <SDL3/SDL_error.h>

pl_status pl_sdl3_gl_init(struct pl_sdl3_ctx *ctx)
{
    if (ctx == nullptr || ctx->window == nullptr)
    {
        return PL_ERROR;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_GLContext gl_ctx = SDL_GL_CreateContext(ctx->window);

    if (gl_ctx == nullptr)
    {
        PL_ASSERT_ERRLOG("failed to create openGL context: %s", SDL_GetError());
        return PL_ERROR;
    }

    SDL_GL_MakeCurrent(ctx->window, gl_ctx);
    SDL_GL_SetSwapInterval(1);

    ctx->backend_ctx = gl_ctx;

    if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress))
    {
        PL_ASSERT_ERRLOG("failed to initialize gl loader: %s", SDL_GetError());
        return PL_ERROR;
    }

    const char *driver = SDL_GetCurrentVideoDriver();

    const GLubyte *version  = glGetString(GL_VERSION);
    const GLubyte *vendor   = glGetString(GL_VENDOR);
    const GLubyte *renderer = glGetString(GL_RENDERER);

    pl_log("Driver: %s", driver ? driver : "N/A");
    pl_log("Vendor: %s | %s", vendor, version);
    pl_log("Renderer: %s", renderer);

    ctx->state |= PL_SDL_STATE_BACKEND_INIT;
    return PL_OK;
}

void pl_sdl3_gl_quit(struct pl_sdl3_ctx *ctx)
{
    if (ctx->backend_ctx)
    {
        SDL_GL_DestroyContext(ctx->backend_ctx);

        ctx->backend_ctx = nullptr;
        ctx->backend     = PL_SDL_BACKEND_NONE;
    }
}

void pl_sdl3_gl_begin(struct pl_sdl3_ctx *ctx, SDL_FColor fcolor)
{
    if (ctx == nullptr || ctx->window == nullptr || ctx->backend_ctx == nullptr ||
        !SDL_GL_MakeCurrent(ctx->window, ctx->backend_ctx))
    {
        return;
    }
    glClearColor(fcolor.r, fcolor.g, fcolor.b, fcolor.a);

    i32 w = ctx->width;
    i32 h = ctx->height;

    glViewport(0, 0, w, h);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void pl_sdl3_gl_end(struct pl_sdl3_ctx *ctx)
{
    if (ctx == nullptr || ctx->window == nullptr || !SDL_GL_SwapWindow(ctx->window))
    {
        return;
    }
}
