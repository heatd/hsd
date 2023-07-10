/* SPDX-License-Identifier: GPL-2.0-ONLY */
#include <page.h>
#include <out.h>

DEFINE_LIST(pagelist);

struct page *pagemap;

void pageinit(unsigned long maxpfn)
{
    printf("pageinit: maxpfn %lx\n", maxpfn);
}

void pagenew(unsigned long start, unsigned long size)
{
    unsigned long pgs = 0, i, addr;

    if (start == 0)
        start += PAGESZ, size -= PAGESZ;
    if (start & (PAGESZ - 1))
    {
        size -= PAGESZ - (start & (PAGESZ-1));
        start = (start + (PAGESZ - 1)) & -PAGESZ;
    }

    if (size & (PAGESZ - 1))
        size &= -PAGESZ;
    
    pgs = size >> PAGESHIFT;

    for (i = 0; i < pgs; i++)
    {
        addr = start + (i << PAGESHIFT);

        if (!pagersv(addr))
        {
            struct page *p = phys_to_page(addr);
            list_add_tail(&p->p.p_alloc, &pagelist);
        }
    }
}

struct page *palloc(int flags)
{
    struct page *p;
    if (list_is_empty(&pagelist))
        return 0;
    
    p = container_of(list_first_element(&pagelist), struct page, p.p_alloc);
    list_remove(&p->p.p_alloc);
    p->p_refs = 1;
    return p;
}
