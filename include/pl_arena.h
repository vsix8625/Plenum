#pragma once

#include "pl_defs.h"

struct pl_arena
{
    void *base;
    u64   capacity;
    u64   used;

    const char *name;
};

struct pl_arena *pl_arena_create(const char *name, u64 capacity);
void             pl_arena_destroy(struct pl_arena *a);

void *pl_arena_push_aligned(struct pl_arena *a, u64 size, u64 align);
void *pl_arena_push(struct pl_arena *a, u64 size);
void *pl_arena_zero_push(struct pl_arena *a, u64 size);
char *pl_arena_strdup(struct pl_arena *a, const char *s);

void pl_arena_reset_soft(struct pl_arena *a);
void pl_arena_reset_hard(struct pl_arena *a);

static inline size_t pl_arena_nxtpow2(size_t v)
{
    if (v == 0)
    {
        return 1;
    }
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v |= v >> 32;
    v++;
    return v;
}

static inline u64 pl_arena_align_up(u64 v, u64 align)
{
    return (v + align - 1) & ~(align - 1);
}
