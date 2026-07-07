#pragma once

#include "pl_defs.h"

#define PL_ARRAY_MAX (PL_U64_MAX / (2 * sizeof(void *)))

struct pl_array
{
    void **elements;

    u64 capacity;
    u64 count;
};

struct pl_array *pl_array_create(u64 initial_capacity);
void             pl_array_destroy(struct pl_array *arr);
bool             pl_array_push(struct pl_array *arr, void *element);
void            *pl_array_pop_at(struct pl_array *arr, u64 idx);
void            *pl_array_pop(struct pl_array *arr);
