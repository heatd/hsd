/* SPDX-License-Identifier: GPL-2.0-ONLY */
#include <stand/port.h>
#include <out.h>

#define COM1 0x3F8

void outc(char c)
{
    /* Deal with \n -> \r\n */
    if (c == '\n')
        outb(COM1, '\r');
    outb(COM1, c);
}

int out(const char *str)
{
    int l = 0;
    char c;
    while ((c = *str++))
    {
        outc(c);
        l++;
    }

    return l;
}

void initserial(void)
{
    printf("HeatSoftwareDistribution\nBest\n");
}
