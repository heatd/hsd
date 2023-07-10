/* SPDX-License-Identifier: GPL-2.0-ONLY */
#include <out.h>

/* output a string to the console */
int
puts(const char *s)
{
    int ret = out(s);
    if (ret < 0)
        return ret;
    outc('\n');
    return ret + 1;
}
