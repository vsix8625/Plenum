#include "pl_sys.h"
#include <stdio.h>
#include <unistd.h>

u64 pl_sys_stream_max(void)
{
    i64 stream_max = sysconf(_SC_STREAM_MAX);
    return (stream_max <= 0) ? FOPEN_MAX : (u64) stream_max;
}

u64 pl_sys_open_max(void)
{
    i64 open_max = sysconf(_SC_OPEN_MAX);

    return (open_max <= 0) ? 256 : (u64) open_max;
}

u64 pl_sys_pagesize(void)
{
    i64 pagesize = sysconf(_SC_PAGESIZE);
    return (pagesize <= 0) ? 4096 : (u64) pagesize;
}

u64 pl_sys_posix_version(void)
{
    i64 version = sysconf(_SC_VERSION);
    return (version <= 0) ? 0 : (u64) version;
}

u64 pl_sys_posix2_version(void)
{
    i64 version = sysconf(_SC_2_VERSION);
    return (version <= 0) ? 0 : (u64) version;
}

u64 pl_sys_phys_pages(void)
{
    i64 val = sysconf(_SC_PHYS_PAGES);
    return (val <= 0) ? 0 : (u64) val;
}

u64 pl_sys_avail_phys_pages(void)
{
    i64 val = sysconf(_SC_AVPHYS_PAGES);
    return (val <= 0) ? 0 : (u64) val;
}

u64 pl_sys_total_phys_memory(void)
{
    return pl_sys_phys_pages() * pl_sys_pagesize();
}

u64 pl_sys_avail_phys_memory(void)
{
    return pl_sys_avail_phys_pages() * pl_sys_pagesize();
}

u64 pl_sys_used_phys_memory(void)
{
    return pl_sys_total_phys_memory() - pl_sys_avail_phys_memory();
}
