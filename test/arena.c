#include "plenum.h"

typedef struct
{
    char *data;
    u64   len;
} sv;

i32 main(void)
{
    if (pl_init() != PL_OK)
    {
        fprintf(stderr, "Failed to init\n");
        return PL_EXIT_FAILURE;
    }

    struct pl_arena *arena = pl_arena_create("test", PL_MiB(2));
    if (arena == nullptr)
    {
        fprintf(stderr, "Failed to create arena\n");
        PL_ASSERT_ERRLOG("Failed to create 'test' arena");
        return PL_EXIT_FAILURE;
    }
    pl_printf("Arena created %s | Size: %lu\n", arena->name, arena->capacity);

    sv *test_alloc = pl_arena_push(arena, sizeof(sv));

    if (test_alloc == nullptr)
    {
        PL_ASSERT_ERRLOG("Failed to push to %s arena", arena->name);
        return PL_EXIT_FAILURE;
    }

    // test over allco

    i32 *oom = pl_arena_zero_push(arena, PL_GiB(1));

    if (oom == nullptr)
    {
        PL_ASSERT_ERRLOG("Failed to push to %s arena", arena->name);
    }
    //----------------

    test_alloc->data = "ok";
    test_alloc->len  = strlen(test_alloc->data);
    pl_printf("Pushed to arena %s | Size: %lu | Used: %lu\n", arena->name, sizeof(sv), arena->used);

    pl_printf("Exit arena system\n");
    pl_quit();

    return PL_EXIT_SUCCESS;
}
