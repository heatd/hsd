/* SPDX-License-Identifier: GPL-2.0-ONLY */
#ifndef _STAND_PORT_H
#define _STAND_PORT_H

static inline void outb(unsigned short port, unsigned char val)
{
    __asm__ __volatile__("outb %0, %1" ::"a"(val), "Nd"(port) : "memory");
}

static inline void outw(unsigned short port, unsigned short val)
{
    __asm__ __volatile__("outw %0, %1" ::"a"(val), "Nd"(port) : "memory");
}

static inline void outl(unsigned short port, unsigned val)
{
    __asm__ __volatile__("outl %0, %1" ::"a"(val), "Nd"(port) : "memory");
}

static inline unsigned char inb(unsigned short port)
{
    unsigned char ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

static inline unsigned short inw(unsigned short port)
{
    unsigned short ret;
    __asm__ __volatile__("inw %1, %0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

static inline unsigned inl(unsigned short port)
{
    unsigned ret;
    __asm__ __volatile__("inl %1, %0" : "=a"(ret) : "Nd"(port) : "memory");
    return ret;
}

static inline void io_wait(void)
{
    /* Port 0x80 is used for 'checkpoints' during POST. */
    /* The Linux kernel seems to think it is free for use :-/ */
    __asm__ __volatile__("outb %%al, $0x80" ::"a"(0) : "memory");
}


#endif
