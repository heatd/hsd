/* SPDX-License-Identifier: GPL-2.0-ONLY */
#include <out.h>

void panic(const char *str)
{
    printf("panic: %s\n", str);
    for (;;)
        __asm__ __volatile__("cli; hlt");
}
