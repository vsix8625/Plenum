#include "pl_array.h"
#include "pl_io.h"

struct pl_array *pl_array_create(u64 initial_capacity)
{
    if (initial_capacity > PL_ARRAY_MAX)
    {
        return nullptr;
    }

    if (initial_capacity == 0)
    {
        initial_capacity = 32;
    }

    struct pl_array *arr = pl_calloc(1, sizeof(struct pl_array));

    if (arr == nullptr)
    {
        return nullptr;
    }

    arr->elements = pl_calloc(initial_capacity, sizeof(void *));

    if (arr->elements == nullptr)
    {
        pl_free(arr);
        return nullptr;
    }

    arr->capacity = initial_capacity;

    return arr;
}

void pl_array_destroy(struct pl_array *arr)
{
    if (arr != nullptr)
    {
        for (u64 i = 0; i < arr->count; i++)
        {
            if (arr->elements)
            {
                pl_free(arr->elements[i]);
            }
        }

        pl_free(arr);
    }
}

bool pl_array_push(struct pl_array *arr, void *element)
{
    if (arr == nullptr)
    {
        return false;
    }

    if (arr->count >= arr->capacity)
    {
        if (arr->capacity > PL_ARRAY_MAX)
        {
            PL_ASSERT_ERRLOG("array reached max capacity");
            return false;
        }

        u64 new_cap  = arr->capacity << 1;
        u64 new_size = sizeof(void *) * new_cap;

        void **tmp = pl_realloc(arr->elements, new_size);

        if (tmp == nullptr)
        {
            PL_ASSERT_ERRLOG("failed to re-allocate array size: %lu", new_cap);
            return false;
        }

        arr->elements = tmp;
        arr->capacity = new_cap;
    }

    arr->elements[arr->count++] = element;

    return true;
}

void *pl_array_pop_at(struct pl_array *arr, u64 idx)
{
    if (arr == nullptr || idx >= arr->count || arr->count == 0)
    {
        return nullptr;
    }

    void *elem = arr->elements[idx];

    // swap with last
    arr->elements[idx] = arr->elements[arr->count - 1];

    arr->elements[arr->count - 1] = nullptr;
    arr->count--;

    return elem;
}

void *pl_array_pop(struct pl_array *arr)
{
    return pl_array_pop_at(arr, arr->count - 1);
}
