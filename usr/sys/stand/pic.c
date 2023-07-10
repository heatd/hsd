/* SPDX-License-Identifier: GPL-2.0-ONLY */
#include <stand/port.h>

#define PIC1 0x20
#define PIC2 0xa0

#define ICW1_SELECT 1 << 4
#define ICW1_ICW4 1 << 0

#define ICW3_CASCADE 1 << 2
#define ICW3_SLAVE 2

#define ICW4_INTEL 1 << 0

static void
maskall(void)
{
    /* mask all irqs but cascade (irq2), since that masks PIC2 as well */
    outb(PIC1 + 1, 0xff & ~(1 << 2));
    io_wait();
    outb(PIC2 + 1, 0xff);
    io_wait();
}

static void
unmask(unsigned irq)
{
    unsigned ctl = PIC1;
    unsigned char mask;
    
    if (irq >= 8)
    {
        ctl = PIC2;
        irq -= 8;
    }

    mask = inb(ctl + 1);
    mask &= ~(1 << irq);

    io_wait();
    outb(ctl + 1, mask);
    io_wait();
}

static void
mask(unsigned irq)
{
    unsigned ctl = PIC1;
    unsigned char mask;
    
    if (irq >= 8)
    {
        ctl = PIC2;
        irq -= 8;
    }

    mask = inb(ctl + 1);
    mask |= (1 << irq);

    io_wait();
    outb(ctl + 1, mask);
    io_wait();
}

#define PIC_EOI 0x20

void
eoi(unsigned irq)
{
    if (irq >= 8)
    {
        outb(PIC2, PIC_EOI);
        io_wait();
    }

    outb(PIC1, PIC_EOI);
}

void picinit(unsigned int irqoff)
{
    /* program the 8259 as specified in the PIIX3 manual. PIC2 is a slave to
      PIC1. We send ICW1, 2, 3, 4 words in sequence, as specified. */
    outb(PIC1, ICW1_SELECT | ICW1_ICW4);
    io_wait();
    outb(PIC2, ICW1_SELECT | ICW1_ICW4);
    io_wait();
    outb(PIC1 + 1, irqoff);
    io_wait();
    outb(PIC2 + 1, irqoff + 8);
    io_wait();
    outb(PIC1 + 1, ICW3_CASCADE);
    io_wait();
    outb(PIC2 + 1, ICW3_SLAVE);
    io_wait();
    outb(PIC1 + 1, ICW4_INTEL);
    io_wait();
    outb(PIC2 + 1, ICW4_INTEL);
    io_wait();

    maskall();
    unmask(0);
    __asm__ __volatile__("sti" ::: "memory");
}
