/* SPDX-License-Identifier: GPL-2.0-ONLY */
#define KVBASE 0xC0000000

.section .bss
.align 4096
.global bootpgd
bootpgd:
.skip 4096 * 2

stack:
.skip 4096 * 4
stack_top:

.section .text.head
jmp i386_unix_main

.align 8
hdr_start: 
    .long 0xe85250d6
    .long 0
    .long hdr_end - hdr_start
    .long 0x100000000 - (0xe85250d6 + 0 + (hdr_end - hdr_start))
.align 8
    # Framebuffer tag
    .word 5
    .word 0
    .long 20
    .long 1024
    .long 768
    .long 32
.align 8
    # Module alignment tag
    .word 6
    .word 0
    .long 8
.align 8
    # Finish tag
    .word 0
    .word 0
    .long 8
hdr_end:

.global i386_unix_main 
i386_unix_main:
    cli
    cld
    /* setup the stack and save the mb2 boot info */
    mov $stack_top, %esp
    sub $KVBASE, %esp
    push %ebx
    push %eax

    mov $bootpgd, %eax
    sub $KVBASE, %eax
    /* Map the first 4MB from 0 onwards to 0xC0000000 (3GB). ecx = pgd, ebx = active pt, edi = index */
    mov %eax, %ecx
    lea 4096(%ecx), %ebx
    xor %edi, %edi

1:
    cmp $1023, %edi
    jg 2f
    mov %edi, %esi
    shl $12, %esi
    or $3, %esi /* P | W */
    mov %esi, (%ebx, %edi, 4) 
    lea 1(%edi), %edi
    jmp 1b
2:
    /* set the pgdir entries */
    or $3, %ebx
    mov %ebx, (%ecx)
    mov %ebx, 3072(%ecx)

    /* enable pg */
    mov %ecx, %cr3
    mov %cr0, %eax
    bts $31, %eax
    mov %eax, %cr0

    /* jump to higher half */
    push $3f
    ret
3:
    /* relocate the stack's address to higher half */
    add $KVBASE, %esp

    call unixinit
    cli
    hlt
