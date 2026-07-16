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
    pl_io_set_prefix(PL_IO_LOG_LEVEL_LOG, "[test_ctor]: ", PL_IO_COLOR_GREEN);

    // pl_log should see that pl_is_initialized() == true and print normally
    struct pl_arena *a = pl_get_global_arena();

    pl_log("Plenum auto initialized");
    pl_log("Global arena: %lu", a->capacity);

    char *token1 = pl_arena_strdup(a, "first string alloc");

    pl_log("Used before scratch: %lu bytes", a->used);
    struct pl_arena_marker tmp_arena_token = pl_arena_marker_begin(a);

    char *token2 = pl_arena_strdup(a, "some string duplicate");
    char *token3 = pl_arena_strdup(a, "another longer string duplicate with pl_arena_strdup");
    pl_log("Tokens: %s | %s | %s", token1, token2, token3);
    pl_log("Used: %lu bytes", a->used);

    pl_arena_marker_end(tmp_arena_token);
    pl_log("Used after scratch: %lu bytes", a->used);

    char *today = pl_time_today(nullptr, 0, PL_TIME_FMT_FILENAME);
    pl_log("Heap date: %s", today);
    pl_free(today);

    char date_buf[128];
    pl_time_today(date_buf, sizeof(date_buf), PL_TIME_FMT_DMY);
    pl_log("Stack date: %s", date_buf);

    // Destructor runs after main
}
