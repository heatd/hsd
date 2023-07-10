/* SPDX-License-Identifier: GPL-2.0-ONLY */
#include <multiboot2.h>
#include <page.h>
#include <out.h>
#include <alloc.h>

void earlymminit(void);

static
void *x86_placement_map(unsigned long addr)
{
    return (void *) addr;
}

struct multiboot_tag_mmap *mmaptag;

static
void multiboot2_parse_tags(unsigned long addr)
{
    struct multiboot_tag *tag;
    struct multiboot_tag *vtag;
    for (tag = (struct multiboot_tag *) (addr + 8),
        vtag = (struct multiboot_tag *) x86_placement_map(addr + 8);
         vtag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag + ((vtag->size + 7) & -8)),
        vtag = (struct multiboot_tag *) x86_placement_map((unsigned long) tag))
    {
        switch (vtag->type)
        {
            case MULTIBOOT_TAG_TYPE_MMAP: {
                mmaptag = (struct multiboot_tag_mmap *) tag;
                break;
            }
        }
    }
}

static
void parsemmap(void)
{
    /* parse the mb2 mmap */
    /* note that we must have this intermediate structure so pagenew doesn't
       corrupt the mb2 structure while we're reading it. */
    struct mmapent
    {
        unsigned long start, len;    
    } mm[128];
    struct mmapent *lastp;
    unsigned long entries, i, nrmm = 0, maxpfn = 0, neededpage;
    struct multiboot_mmap_entry *ment;

    entries = mmaptag->size / mmaptag->entry_size;

    for (ment = mmaptag->entries, i = 0; i < entries; ment++, i++)
    {
        if (ment->type != MULTIBOOT_MEMORY_AVAILABLE)
            continue;
        if (ment->addr == 0)
            ment->addr += PAGESZ, ment->len -= PAGESZ;
        mm[nrmm].start = ment->addr;
        mm[nrmm++].len = ment->len;

        if (ment->addr + ment->len > maxpfn)
            maxpfn = ment->addr + ment->len;
    }

    maxpfn >>= PAGESHIFT;

    /* Carve a bit of memory out for page structs.
       We use the last mm as its the most likely one to be large.
       This is naive.
     */
    neededpage = maxpfn * sizeof(struct page); 
    lastp = &mm[nrmm - 1];

    if (lastp->len < neededpage)
        panic("no memory for per-page data");
    
    /* XXX check if the kernel colides with this */

    pagemap = (struct page *) (KVBASE + lastp->start + lastp->len - neededpage);
    lastp->len -= neededpage;
    lastp->len &= -PAGESZ;

    pageinit(maxpfn);

    for (i = 0; i < nrmm; i++)
    {
        pagenew(mm[i].start, mm[i].len);
    }

} 

void trapinit(void);
void initserial(void);

void unixinit(unsigned int magic, unsigned int addr)
{
    initserial();
    multiboot2_parse_tags(addr);
    earlymminit();
    trapinit();
    parsemmap();

    for (;;)
        __asm__("hlt");
}
