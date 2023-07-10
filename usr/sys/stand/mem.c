/* SPDX-License-Identifier: GPL-2.0-ONLY */
#include <page.h>

#define NPTE 1024
#define NIDXBITS 10

#define PG_P 1
#define PG_W 2
#define PG_U 4

extern char kernel_phys_start[];
extern char kernel_phys_end[];

/* check if page is reserved */
int pagersv(unsigned long page)
{
    return page >= (unsigned long) &kernel_phys_start && page < (unsigned long) &kernel_phys_end;
}

struct pt
{
    unsigned int entry[NPTE];
};

#define PTE3G 768
#define RESVPGDE 32
#define NDIRMAPTAB (NPTE - PTE3G - RESVPGDE)

extern struct pt bootpgd;

struct pt directmap[NDIRMAPTAB - 1] __attribute__((aligned(4096)));

void earlymminit(void)
{
    /* map the "whole memory" to 3GB */
    int i, j;
    unsigned long addr = 1 << (PAGESHIFT + NIDXBITS);
    for (i = 0; i < NDIRMAPTAB - 1; i++)
    {
        struct pt* pt = &directmap[i];

        for (j = 0; j < NPTE; j++)
        {
            pt->entry[j] = addr | PG_P | PG_W;
            addr += PAGESZ;
        }

        bootpgd.entry[PTE3G + 1 + i] = ((unsigned long) pt - KVBASE) | PG_P | PG_W;
    }
}
