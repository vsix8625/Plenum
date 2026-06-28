#include "../include/plenum.h"
#include <stdio.h>

i32 main(i32 argc, char **argv)
{
    (void) argc;
    (void) argv;
    if (pl_is_initialized())
    {
        printf("Plenum initialized!\n");
    }
    else
    {
        printf("Plenum NOT initialized!\n");
    }
    pl_init();

    if (pl_arena_init() < 0)
    {
        return PL_EXIT_FAILURE;
    }

    if (pl_is_initialized())
    {
        printf("Plenum initialized!\n");
    }
    else
    {
        printf("Plenum NOT initialized!\n");
    }

    pl_quit();
    printf("Plenum quit!\n");

    if (pl_is_initialized())
    {
        printf("Plenum initialized!\n");
    }
    else
    {
        printf("Plenum NOT initialized!\n");
    }

    return PL_EXIT_SUCCESS;
}
