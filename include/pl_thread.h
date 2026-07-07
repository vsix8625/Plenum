#pragma once

#include "pl_defs.h"
#include <bits/pthreadtypes.h>

typedef pthread_mutex_t pl_mutex;
typedef pthread_cond_t  pl_cond;

typedef void *(*pl_thread_fn)(void *);
struct pl_thread;

pl_status pl_thread_create(struct pl_thread *thrd, pl_thread_fn fn, void *arg);
pl_status pl_thread_join(struct pl_thread *thrd);
pl_status pl_thread_detach(struct pl_thread *thrd);

u64 pl_thread_id(void);

void pl_mutex_init(pl_mutex *mx);
void pl_mutex_destroy(pl_mutex *mx);
void pl_mutex_lock(pl_mutex *mx);
void pl_mutex_unlock(pl_mutex *mx);
void pl_cond_init(pl_cond *cond);
void pl_cond_destroy(pl_cond *cond);
void pl_cond_signal(pl_cond *cond);
void pl_cond_broadcast(pl_cond *cond);
void pl_cond_wait(pl_cond *cond, pl_mutex *mx);
