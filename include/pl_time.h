#pragma once

#include "pl_defs.h"

struct pl_ticks
{
    u64 start, end;
};

/*
 * Get the number of nanoseconds since Plenum library initialization
 */
u64 pl_time_ns(void);

/*
 * Get the number of microseconds since Plenum library initialization
 */
u64 pl_time_micro(void);

/*
 * Get the number of millieseconds since Plenum library initialization
 */
u64 pl_time_ms(void);

/*
 * Get the number of seconds (`float`) since Plenum library initialization.
 */
f32 pl_time_f32(void);

/*
 * Get the number of seconds (`double`) since Plenum library initialization.
 */
f64 pl_time_f64(void);

/*
 * Captures high-resolution timestamp and store it as the starting point in a `pl_ticks` structure.
 * Resets the `end` value to 0.
 */
void pl_ticks_start(struct pl_ticks *t);

/*
 * Captures high-resolution timestamp and store it as the end point in a `pl_ticks` structure.
 * When paired with a preceding `pl_ticks_start` call, this defines the duration of the profiled
 * block.
 */
void pl_ticks_end(struct pl_ticks *t);

u64 pl_time_epoch_sec(void);

/*
 * Converts the duration between the start and end ticks into a human-readable string.
 * Returns a pointer to the provided buffer.
 */
char *pl_time_ticks_format(const struct pl_ticks *ticks, char *buf, size_t buf_size);
