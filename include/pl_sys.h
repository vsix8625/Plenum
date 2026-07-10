#pragma once

#include "pl_defs.h"

u64 pl_sys_stream_max(void);
u64 pl_sys_open_max(void);
u64 pl_sys_pagesize(void);
u64 pl_sys_posix_version(void);
u64 pl_sys_posix2_version(void);
u64 pl_sys_phys_pages(void);
u64 pl_sys_avail_phys_pages(void);

u64 pl_sys_total_phys_memory(void);
u64 pl_sys_avail_phys_memory(void);
u64 pl_sys_used_phys_memory(void);
