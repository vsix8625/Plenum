#include "pl_thread.h"
#include "pl_io.h"

#include <pthread.h>
#include <stdatomic.h>

struct pl_thread
{
    pthread_t    handle;
    pl_thread_fn fn;
    void        *arg;
    atomic_bool  running;
};

static pl_status pl_thread_join(struct pl_thread *thrd);

bool pl_thread_is_running(struct pl_thread *thrd)
{
    if (thrd == nullptr)
    {
        return false;
    }

    return atomic_load(&thrd->running);
}

static void *pl_thread_entry(void *arg)
{
    struct pl_thread *thrd = (struct pl_thread *) arg;

    thrd->fn(thrd->arg);

    atomic_store(&thrd->running, false);

    return nullptr;
}

struct pl_thread *pl_thread_create(pl_thread_fn fn, void *arg)
{
    if (fn == nullptr)
    {
        return nullptr;
    }

    struct pl_thread *t = pl_calloc(1, sizeof(struct pl_thread));

    if (t == nullptr)
    {
        PL_ASSERT_ERRLOG("failed to allocate for pl_thread");
        return nullptr;
    }

    t->fn  = fn;
    t->arg = arg;
    atomic_store(&t->running, true);

    i32 rc = pthread_create(&t->handle, nullptr, pl_thread_entry, t);
    if (rc != PL_OK)
    {
        errno = rc;
        PL_ASSERT_ERRLOG("failed to create thread");
        pl_free(t);
        return nullptr;
    }

    return t;
}

void pl_thread_destroy(struct pl_thread *thrd)
{
    if (thrd)
    {
        pl_thread_join(thrd);
        pl_free(thrd);
    }
}

static pl_status pl_thread_join(struct pl_thread *thrd)
{
    i32 rc = pthread_join(thrd->handle, nullptr);
    if (rc != PL_OK)
    {
        errno = rc;
        PL_ASSERT_ERRLOG("failed to join thread");
        return PL_ERROR;
    }
    return PL_OK;
}

u64 pl_thread_id(void)
{
    return (u64) pthread_self();
}

void pl_mutex_init(pl_mutex *mx)
{
    i32 rc = pthread_mutex_init(mx, nullptr);
    if (rc != PL_OK)
    {
        errno = rc;
        PL_ASSERT_ERRLOG("failed to initialize mutex");
        return;
    }
}

void pl_mutex_destroy(pl_mutex *mx)
{
    i32 rc = pthread_mutex_destroy(mx);
    if (rc != PL_OK)
    {
        errno = rc;
        PL_ASSERT_ERRLOG("failed to destroy mutex");
        return;
    }
}

void pl_mutex_lock(pl_mutex *mx)
{
    i32 rc = pthread_mutex_lock(mx);
    if (rc != PL_OK)
    {
        errno = rc;
        PL_ASSERT_ERRLOG("failed to lock mutex");
        return;
    }
}

void pl_mutex_unlock(pl_mutex *mx)
{
    i32 rc = pthread_mutex_unlock(mx);
    if (rc != PL_OK)
    {
        errno = rc;
        PL_ASSERT_ERRLOG("failed to unlock mutex");
        return;
    }
}

void pl_cond_init(pl_cond *cond)
{
    i32 rc = pthread_cond_init(cond, nullptr);
    if (rc != PL_OK)
    {
        errno = rc;
        PL_ASSERT_ERRLOG("failed to initialize pthread cond");
        return;
    }
}

void pl_cond_destroy(pl_cond *cond)
{
    i32 rc = pthread_cond_destroy(cond);
    if (rc != PL_OK)
    {
        errno = rc;
        PL_ASSERT_ERRLOG("failed to destroy pthread cond");
        return;
    }
}

void pl_cond_signal(pl_cond *cond)
{
    i32 rc = pthread_cond_signal(cond);
    if (rc != PL_OK)
    {
        errno = rc;
        PL_ASSERT_ERRLOG("failed to signal pthread cond");
        return;
    }
}

void pl_cond_broadcast(pl_cond *cond)
{
    i32 rc = pthread_cond_broadcast(cond);
    if (rc != PL_OK)
    {
        errno = rc;
        PL_ASSERT_ERRLOG("failed to broadcast pthread cond");
        return;
    }
}

void pl_cond_wait(pl_cond *cond, pl_mutex *mx)
{
    i32 rc = pthread_cond_wait(cond, mx);
    if (rc != PL_OK)
    {
        errno = rc;
        PL_ASSERT_ERRLOG("failed to wait");
        return;
    }
}
