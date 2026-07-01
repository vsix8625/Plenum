#include "pl_string.h"

pl_sv pl_sv_from_cstr(const char *cstr)
{
    pl_sv sv = {0};

    if (cstr == nullptr || cstr[0] == '\0')
    {
        return sv;
    }

    sv.data = cstr;
    sv.len  = strlen(cstr);

    return sv;
}

i32 pl_sv_strcmp(pl_sv a, pl_sv b)
{
    if (a.len != b.len)
    {
        return (a.len > b.len) ? -1 : 1;
    }

    return memcmp(a.data, b.data, a.len);
}
