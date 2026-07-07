#include "pl_time.h"
#include <stdio.h>
#include <time.h>

static u64 g_pl_time_start = 0;

static u64 get_system_ns(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (u64) ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

void pl_time_init(void)
{
    g_pl_time_start = get_system_ns();
}

u64 pl_time_ns(void)
{
    return get_system_ns() - g_pl_time_start;
}

u64 pl_time_micro(void)
{
    return pl_time_ns() / 1000ULL;
}

u64 pl_time_ms(void)
{
    return pl_time_ns() / 1000000ULL;
}

f32 pl_time_f32(void)
{
    return (f32) ((f64) pl_time_ns() * 0.000000001);
}

f64 pl_time_f64(void)
{
    return (f64) pl_time_ns() * 0.000000001;
}

void pl_ticks_start(struct pl_ticks *t)
{
    t->start = pl_time_ns();
    t->end   = 0;
}

void pl_ticks_end(struct pl_ticks *t)
{
    t->end = pl_time_ns();
}

u64 pl_time_epoch_sec(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == 0)
    {
        return (u64) ts.tv_sec;
    }

    return 0;
}

char *pl_time_ticks_format(const struct pl_ticks *ticks, char *buf, size_t buf_size)
{
    if (ticks == nullptr || buf == nullptr || buf_size == 0)
    {
        return buf;
    }

    u64 diff = ticks->end - ticks->start;
    f64 ns   = (f64) diff;

    if (diff < 1000ULL)
    {
        snprintf(buf, buf_size, "%lu ns", (u64) diff);
    }
    else if (diff < 1000000ULL)
    {
        snprintf(buf, buf_size, "%.2f us", ns * 0.001);
    }
    else if (diff < 1000000000ULL)
    {
        snprintf(buf, buf_size, "%.2f ms", ns * 0.000001);
    }
    else if (diff < 60000000000ULL)
    {
        snprintf(buf, buf_size, "%.3f s", ns * 0.000000001);
    }
    else
    {
        snprintf(buf, buf_size, "%.2f min", ns * 1.6666666666666667e-11);
    }

    return buf;
}
