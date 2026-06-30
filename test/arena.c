#include "../include/plenum.h"
#include <stdio.h>
#include <string.h>

typedef struct
{
    char *data;
    u64   len;
} sv;

i32 main(void)
{
    i32 result = PL_EXIT_SUCCESS;
    pl_init();

    if (pl_arena_init() != PL_OK)
    {
        result = PL_EXIT_FAILURE;
    }
    printf("Arena system initialized\n");

    struct pl_arena *arena = pl_arena_create("test", PL_MiB(2));
    if (arena == nullptr)
    {
        result = PL_EXIT_FAILURE;
    }
    printf("Arena created %s | Size: %lu\n", arena->name, arena->capacity);

    sv *test_alloc = pl_arena_push(arena, sizeof(sv));

    if (test_alloc == nullptr)
    {
        result = PL_EXIT_FAILURE;
    }

    test_alloc->data = "ok";
    test_alloc->len  = strlen(test_alloc->data);
    printf("Pushed to arena %s | Size: %lu | Used: %lu\n", arena->name, sizeof(sv), arena->used);

    pl_arena_quit();
    printf("Exit arena system\n");
    pl_quit();
    return result;
}
