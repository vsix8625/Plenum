#pragma once

#include "pl_defs.h"

typedef struct pl_sv_s
{
    const char *data;
    u64         len;
} pl_sv;

#define PL_SV(s) {s, sizeof(s) - 1}

pl_sv pl_sv_from_cstr(const char *cstr);

// compare two string views
i32 pl_sv_strcmp(pl_sv a, pl_sv b);
