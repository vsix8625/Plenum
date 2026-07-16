#pragma once

#include "pl_defs.h"

typedef enum
{
    PL_TIME_FMT_DATETIME,  // 2026-07-12 18:45:57
    PL_TIME_FMT_DMY,       // 12-07-2026 (Euro / UK / DD-MM-YYYY)
    PL_TIME_FMT_MDY,       // 07-12-2026 (US / MM-DD-YYYY)
    PL_TIME_FMT_YMD,       // 2026-07-12
    PL_TIME_FMT_HMS,       // 18:45:57
    PL_TIME_FMT_FILENAME,  // 20260712_184557
} pl_time_fmt;

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

/*
 * If `out_buffer` is nullptr it will return a heap allocated string.
 */
char *pl_time_today(char *out_buffer, u64 out_buffer_size, pl_time_fmt fmt_type);
