#include "pl_cpu.h"
#include <unistd.h>

u64 pl_cpu_nproc(void)
{
    i64 n = sysconf(_SC_NPROCESSORS_ONLN);

    return n < 1 ? 1 : (u64) n;
}

u64 pl_cpu_cacheline(void)
{
    i64 size = sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
    return (size <= 0) ? 64 : (u64) size;
}
