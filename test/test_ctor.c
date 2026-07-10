#include "plenum.h"

i32 main(void)
{
    // If PL_USE_CONSTRUCTOR was set and worked, pl_init() already ran behind the scenes
    if (!pl_is_initialized())
    {
        // if its false, exit
        fprintf(stderr, "Plenum is not initialized\n");
        return PL_EXIT_FAILURE;
    }

    // pl_log should see that pl_is_initialized() == true and print normally
    struct pl_arena *a = pl_get_global_arena();

    pl_log("Plenum auto initialized");
    pl_log("Global arena: %lu", a->capacity);
    pl_log("Used before scratch: %lu bytes", a->used);

    struct pl_arena_tmp tmp_arena = pl_arena_tmp_begin(a);

    char *token1 = pl_arena_strdup(a, "some string duplicate");
    char *token2 = pl_arena_strdup(a, "another longer string duplicate with pl_arena_strdup");
    pl_log("Tokens: %s | %s", token1, token2);
    pl_log("Used: %lu bytes", a->used);

    pl_arena_tmp_end(tmp_arena);
    pl_log("Used after scratch: %lu bytes", a->used);

    // Destructor runs after main
}
