/* SPDX-License-Identifier: GPL-2.0-ONLY */
#include <page.h>
#include <alloc.h>
#include <out.h>

/* Implementation of malloc/free/realloc/calloc using the McKusick-Karels algo.
 * We limit allocations to the PAGESZ.
 * McKusick-Karels is a power-of-2 allocator where we keep lists of pages for each
 * power of 2. Those pages keep the metadata for the actual size of each allocation (so
 * we know where to put the allocation on free()).
 */

/* Max order is page order, min order is log2(sizeof(void *)), 2 on 32-bit systems */
#define MAXORDER PAGESHIFT
#define MINORDER 2

#define NORDERS MAXORDER - MINORDER

struct freelist
{
    void *chunk_head;
    unsigned int f_totalbytes;
    unsigned int f_allocbytes;
};

#define ilog2(X)           ((unsigned) (8 * sizeof(unsigned long) - __builtin_clzl((X)) - 1))

struct freelist kmemfree[NORDERS];

int mem_to_order(size_t len)
{
    int order;
    if (len < sizeof(void *))
        return 0;

    order = ((int) ilog2(len - 1) + 1) - MINORDER;

    if (order < 0)
        order = 0;
    return order;
}

struct page *virt_to_page(void *p);

int growfl(struct freelist *flp, int order)
{
    unsigned int bsize;
    int nblocks, i;
    char *virt;
    struct page *p;
    
    bsize = 1U << (order + MINORDER);
    nblocks = PAGESZ / bsize;
    p = palloc(P_WAITOK);

    if (!p)
        return 0;
    p->p.p_mdata.m_bsize = bsize;

    virt = page_to_virt(p);

    for (i = 0; i < nblocks; i++, virt += bsize)
    {
        *(void **) virt = flp->chunk_head;
        flp->chunk_head = virt;
    }

    flp->f_totalbytes += PAGESZ;

    return 0;
}

void *malloc(size_t len)
{
    int order;
    struct freelist *flp;
    void *chunk;

    order = mem_to_order(len);

    if (order > MAXORDER)
        return 0;
    
    flp = &kmemfree[order];

    if (!flp->chunk_head)
    {
        if (growfl(flp, order) < 0)
            return 0;
    }

    chunk = flp->chunk_head;
    flp->f_allocbytes += 1U << (order + MINORDER);
    flp->chunk_head = *(void **) chunk;

    return chunk;
}

void free(void *p)
{
    unsigned short bsize;
    int order;
    struct freelist *flp;
    struct page *page;
 
    if (!p)
        return;

    page = virt_to_page(p);

    if (!page)
        panic("free: bad pointer");
    
    bsize = page->p.p_mdata.m_bsize;

    if (!bsize)
        panic("free: given pointer is not part of the heap");

    if ((unsigned long) p & (bsize - 1))
        panic("free: bad pointer");
    
    order = mem_to_order(bsize);
    flp = &kmemfree[order];
    flp->f_allocbytes -= bsize;

    *(void **) p = flp->chunk_head;
    flp->chunk_head = p;
}

#if 0
void *calloc(size_t nmemb, size_t len)
{

}
#endif
