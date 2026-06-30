#include "pl_arena.h"
#include "pl_init.h"

#include <string.h>
#include <assert.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

#define PL_ARENA_ADDR_HINT      0x700000000000ULL
#define PL_ARENA_PAGE_SIZE_HUGE 0x00200000 /* 2MB huge pages */

enum pl_arena_state : i32
{
    PL_ARENA_STATE_OFF           = 0,
    PL_ARENA_STATE_ON            = 1,
    PL_ARENA_STATE_SHUTTING_DOWN = 2,
};

static _Atomic enum pl_arena_state g_pl_arena_state  = PL_ARENA_STATE_OFF;
static _Atomic uptr                g_pl_arena_cursor = 0;
static u64                         g_pl_sys_pagesize = 0;

struct pl_arena_registry
{
    struct pl_arena **arenas;

    u64 count, capacity;

    atomic_flag lock;
};

static struct pl_arena_registry g_pl_arena_registry = {0};

// ----------------------------------------------------------------------------------------------------

static void registry_lock(void)
{
    while (atomic_flag_test_and_set_explicit(&g_pl_arena_registry.lock, memory_order_acquire))
    {
#if defined(__x86_64__) || defined(_M_X64)
        __builtin_ia32_pause();
#endif
    }
}

static void registry_unlock(void)
{
    atomic_flag_clear_explicit(&g_pl_arena_registry.lock, memory_order_release);
}

static void registry_add(struct pl_arena *a)
{
    if (a == nullptr)
    {
        return;
    }

    registry_lock();

    if (g_pl_arena_registry.count >= g_pl_arena_registry.capacity)
    {
        u64 new_cap = g_pl_arena_registry.capacity ? g_pl_arena_registry.capacity * 2 : 128;

        struct pl_arena **tmp_arenas =
            pl_realloc(g_pl_arena_registry.arenas, new_cap * sizeof(struct pl_arena *));

        if (tmp_arenas == nullptr)
        {
            // NOTE: log error
            registry_unlock();
            return;
        }

        g_pl_arena_registry.arenas   = tmp_arenas;
        g_pl_arena_registry.capacity = new_cap;
    }

    g_pl_arena_registry.arenas[g_pl_arena_registry.count++] = a;
    registry_unlock();
}

static void registry_remove(struct pl_arena *a)
{
    if (a == nullptr)
    {
        return;
    }

    registry_lock();

    for (u64 i = 0; i < g_pl_arena_registry.count; i++)
    {
        struct pl_arena *current_arena = g_pl_arena_registry.arenas[i];

        if (current_arena == a)
        {
            g_pl_arena_registry.arenas[i] = g_pl_arena_registry.arenas[--g_pl_arena_registry.count];
            break;
        }
    }

    registry_unlock();
}

// ----------------------------------------------------------------------------------------------------

pl_status pl_arena_init(void)
{
    if (!pl_is_initialized())
    {
        return PL_FATAL;
    }

    enum pl_arena_state expected_state = PL_ARENA_STATE_OFF;
    if (!atomic_compare_exchange_strong(&g_pl_arena_state, &expected_state, PL_ARENA_STATE_ON))
    {
        // NOTE: log_error
        return PL_FATAL;
    }

    g_pl_sys_pagesize = (u64) sysconf(_SC_PAGESIZE);

    uptr cursor_expected = 0;
    uptr hint            = PL_ARENA_ADDR_HINT;
    atomic_compare_exchange_strong(&g_pl_arena_cursor, &cursor_expected, hint);

    return PL_OK;
}

void pl_arena_quit(void)
{
    enum pl_arena_state expected_state = PL_ARENA_STATE_ON;
    if (!atomic_compare_exchange_strong(
            &g_pl_arena_state, &expected_state, PL_ARENA_STATE_SHUTTING_DOWN))
    {
        // NOTE: log_error
        return;
    }

    // destroy all registred arenas
    u64 n_count = g_pl_arena_registry.count;
    while (n_count--)
    {
        struct pl_arena *a = g_pl_arena_registry.arenas[0];
        (void) a;
        // destroy arena fn
    }

    pl_free(g_pl_arena_registry.arenas);

    g_pl_arena_registry.arenas   = nullptr;
    g_pl_arena_registry.count    = 0;
    g_pl_arena_registry.capacity = 0;
}

//----------------------------------------------------------------------------------------------------

struct pl_arena *pl_arena_create(const char *name, u64 capacity)
{
    if (name == nullptr || name[0] == '\0')
    {
        name = "plenum_arena";
    }

    if (capacity == 0)
    {
        // NOTE:
        return nullptr;
    }

    capacity = pl_arena_nxtpow2(capacity);

    u64 page_alignment = g_pl_sys_pagesize;

    if (capacity >= PL_ARENA_PAGE_SIZE_HUGE)
    {
        page_alignment = PL_ARENA_PAGE_SIZE_HUGE;
    }

    capacity = pl_arena_align_up(capacity, page_alignment);

    uptr hint = atomic_fetch_add(&g_pl_arena_cursor, capacity);

    struct pl_arena *arena = pl_calloc(1, sizeof(struct pl_arena));
    if (arena == nullptr)
    {
        // NOTE:

        return nullptr;
    }

    void *ptr =
        mmap((void *) hint, capacity, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (ptr == MAP_FAILED)
    {
        // retry with 0 hint
        ptr = mmap(0, capacity, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        if (ptr == MAP_FAILED)
        {
            // NOTE: log error
            pl_free(arena);
            return nullptr;
        }
    }

    arena->name     = name;
    arena->base     = ptr;
    arena->capacity = capacity;
    arena->used     = 0;

    if (page_alignment == PL_ARENA_PAGE_SIZE_HUGE)
    {
        madvise(arena->base, arena->capacity, MADV_HUGEPAGE);
    }

    registry_add(arena);
    return arena;
}

void pl_arena_destroy(struct pl_arena *a)
{
    if (a == nullptr)
    {
        return;
    }

    registry_remove(a);

    if (a->base)
    {
        if (munmap(a->base, a->capacity) != 0)
        {
            // NOTE: unmap failed
            return;
        }
        a->base = nullptr;
    }

    pl_free(a);
}

void *pl_arena_push_aligned(struct pl_arena *a, u64 size, u64 align)
{
    if (a == nullptr || size == 0)
    {
        return nullptr;
    }

    assert((align & (align - 1)) == 0 && "align must be a power of two");

    if (align < 16)
    {
        align = 16;
    }

    u64 start = (a->used + align - 1) & ~(align - 1);

    if (start + size > a->capacity)
    {
        // NOTE: log oom error;
        return nullptr;
    }

    void *ptr = (byte *) a->base + start;
    a->used   = start + size;

    return ptr;
}

void *pl_arena_push(struct pl_arena *a, u64 size)
{
    return pl_arena_push_aligned(a, size, 16);
}

void *pl_arena_zero_push(struct pl_arena *a, u64 size)
{
    void *ptr = pl_arena_push_aligned(a, size, 16);
    if (ptr)
    {
        memset(ptr, 0, size);
    }

    return ptr;
}

void pl_arena_reset_soft(struct pl_arena *a)
{
    if (a == nullptr)
    {
        return;
    }

    a->used = 0;
}

void pl_arena_reset_hard(struct pl_arena *a)
{
    if (a == nullptr || a->used == 0)
    {
        return;
    }

    madvise(a->base, a->used, MADV_DONTNEED);

    a->used = 0;
}
