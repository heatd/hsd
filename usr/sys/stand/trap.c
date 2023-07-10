/* SPDX-License-Identifier: GPL-2.0-ONLY */
#include <stand/seg.h>
#include <out.h>

void loadidt(unsigned short limit, void *);

struct idte
{
    unsigned short idt_offlo;
    unsigned short idt_segsel;
    unsigned char idt_mbz;
    unsigned char idt_flags;
    unsigned short idt_offhi;
} __attribute__((packed));

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

extern void (*x86_isr_table[])(void);

static
void initidt(void)
{
    int i;

    for (i = 0; i < 256; i++)
        trapresv(i, x86_isr_table[i]);

    /* Double fault handlers use a separate stack */
    /* TODO: Set this up better. */
    //idt_entries[8].zero = 1;
    loadidt(sizeof(idt) - 1, idt);
}

void initgdt(void);
void picinit(unsigned int irqoff);
void pitinit(unsigned frequency);

void trapinit(void)
{
    initgdt();
    initidt();
    picinit(32);
    pitinit(100);
}

struct uregs
{
    unsigned ds;
    unsigned ebp;
    unsigned esi;
    unsigned edi;
    unsigned edx;
    unsigned ecx;
    unsigned ebx;
    unsigned eax;
    unsigned trapno, trap_errc;
    unsigned eip;
    unsigned cs;
    unsigned eflags;
    unsigned esp;
    unsigned ss;
};


void eoi(unsigned irq);

struct uregs *trapdisp(struct uregs *reg)
{
    if (reg->trapno < 32)
        panic("exception"); /* XXX signals? */
    if (reg->trapno >= 32)
        eoi(reg->trapno - 32);
    return reg;
}
