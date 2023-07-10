/* SPDX-License-Identifier: GPL-2.0-ONLY */
#include <stand/seg.h>

void loadidt(unsigned short limit, void *);

struct idte
{
    unsigned short idt_offlo;
    unsigned short idt_segsel;
    unsigned char idt_mbz;
    unsigned char idt_flags;
    unsigned char idt_offhi;
};

/* 0x8e = P | 0 D 1 1 0
 * P = segment present,
 * D = 1 = 32 bits, 0 = 16
 * DPL = 0
 * 110 codifies an interrupt gate (111 is a trap gate). We want interrupt gates for the auto-cli.
 */
#define INTGATE_FLAGS 0x8e

static struct idte idt[256];

static
void trapresv(unsigned char vec, void (*func)(void))
{
    unsigned long addr = (unsigned long) func;
    idt[vec].idt_offlo = addr & 0xffff;
    idt[vec].idt_offhi = addr >> 16;
    idt[vec].idt_mbz = 0;
    idt[vec].idt_flags = INTGATE_FLAGS;
    idt[vec].idt_segsel = KERNEL_CS;
}

extern void Xisr0(void);
extern void Xisr1(void);
extern void Xisr2(void);
extern void Xisr3(void);
extern void Xisr4(void);
extern void Xisr5(void);
extern void Xisr6(void);
extern void Xisr7(void);
extern void Xisr8(void);
extern void Xisr9(void);
extern void Xisr10(void);
extern void Xisr11(void);
extern void Xisr12(void);
extern void Xisr13(void);
extern void Xisr14(void);
extern void Xisr15(void);
extern void Xisr16(void);
extern void Xisr17(void);
extern void Xisr18(void);
extern void Xisr19(void);
extern void Xisr20(void);
extern void Xisr21(void);
extern void Xisr22(void);
extern void Xisr23(void);
extern void Xisr24(void);
extern void Xisr25(void);
extern void Xisr26(void);
extern void Xisr27(void);
extern void Xisr28(void);
extern void Xisr29(void);
extern void Xisr30(void);
extern void Xisr31(void);


static
void initidt(void)
{
    trapresv(0, Xisr0);
    trapresv(1, Xisr1);
    trapresv(2, Xisr2);
    trapresv(3, Xisr3);
    trapresv(4, Xisr4);
    trapresv(5, Xisr5);
    trapresv(6, Xisr6);
    trapresv(7, Xisr7);
    trapresv(8, Xisr8);
    trapresv(9, Xisr9);
    trapresv(10, Xisr10);
    trapresv(11, Xisr11);
    trapresv(12, Xisr12);
    trapresv(13, Xisr13);
    trapresv(14, Xisr14);
    trapresv(15, Xisr15);
    trapresv(16, Xisr16);
    trapresv(17, Xisr17);
    trapresv(18, Xisr18);
    trapresv(19, Xisr19);
    trapresv(20, Xisr20);
    trapresv(21, Xisr21);
    trapresv(22, Xisr22);
    trapresv(23, Xisr23);
    trapresv(24, Xisr24);
    trapresv(25, Xisr25);
    trapresv(26, Xisr26);
    trapresv(27, Xisr27);
    trapresv(28, Xisr28);
    trapresv(29, Xisr29);
    trapresv(30, Xisr30);
    trapresv(31, Xisr31);

    /* Double fault handlers use a separate stack */
    /* TODO: Set this up better. */
    //idt_entries[8].zero = 1;
    loadidt(sizeof(idt) - 1, idt);
}

void initgdt(void);

void trapinit(void)
{
    initgdt();
    initidt();
}
