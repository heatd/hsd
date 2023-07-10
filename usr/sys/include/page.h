/* SPDX-License-Identifier: GPL-2.0-ONLY */
#ifndef _PAGEALLOC_H
#define _PAGEALLOC_H

#include <list.h>

#define PAGESZ 4096ul
#define PAGESHIFT 12

#define KVBASE 0xc0000000

struct page
{
    unsigned int p_refs;
    /* for malloc, we keep the block size and free blocks as data in struct page */
    union
    {
        struct {
            unsigned short m_bsize;
            unsigned short m_freeblk;
        } p_mdata;

        struct list_head p_alloc;
    } p;
};

extern struct page *pagemap;

#define phys_to_page(phys) (pagemap + ((unsigned long)(phys) >> PAGESHIFT))
#define phys_to_virt(phys) ((void *) (((unsigned long) (phys)) + KVBASE))
#define page_to_virt(page) phys_to_virt((page - pagemap) << PAGESHIFT)

void pageinit(unsigned long maxpfn);
void pagenew(unsigned long start, unsigned long size);

/* check if page is reserved */
int pagersv(unsigned long page);

#define P_WAITOK (1 << 0)

struct page *palloc(int flags);

void pfree(struct page *p);

#endif
