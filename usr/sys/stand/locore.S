/*
 * Copyright (c) 2016 - 2023 Pedro Falcato
 * This file is part of Onyx, and is released under the terms of the GPLv2 License
 * check LICENSE at the root directory for more information
 *
 * SPDX-License-Identifier: GPL-2.0-ONLY
 */
#include <stand/seg.h>
.text

.global loadgdt
loadgdt:
    mov 8(%esp), %eax
    mov 4(%esp), %ecx
    pushl %eax
    pushw %cx
    lgdtl (%esp)
    add $6, %esp

    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %ss
    mov %ax, %es
    mov %ax, %gs
    mov %ax, %fs
    ljmp $0x8, $1f
1:
    ret

.global loadidt
loadidt:
    mov 8(%esp), %eax
    mov 4(%esp), %ecx
    pushl %eax
    pushw %cx
    lidtl (%esp)
    add $6, %esp
    ret

.macro pushall
    push %eax
    push %ebx
    push %ecx
    push %edx
    push %edi
    push %esi
    push %ebp
.endm

.macro popall
    pop %ebp
    pop %esi
    pop %edi
    pop %edx
    pop %ecx
    pop %ebx
    pop %eax
.endm

/* struct registers has two longs that are used only for ISRs,
 and meaningless otherwise
*/
#define REGISTERS_UNUSED_OFF		8

#define EXCEPTION_VECTORS_END       32

.macro ISR_NOERRCODE num
.global Xisr\num
.type Xisr\num,@function
Xisr\num:
    cli
    sub $4, %esp
    push $\num # Push the interrupt number
    jmp Xinterrupt_common # Go to the handler

.pushsection .rodata.isr_table
.long Xisr\num
.popsection

.endm

.macro IRQ irq_num
.global Xirq\irq_num
.type Xirq\irq_num,@ function
Xirq\irq_num:
    sub $4, %esp
    push $(\irq_num + EXCEPTION_VECTORS_END)
    jmp Xinterrupt_common

.pushsection .rodata.isr_table
.long Xirq\irq_num
.popsection

.endm

.macro ISR_ERRCODE num
.global Xisr\num
Xisr\num:
    push $\num # Push the interrupt number to the intctx
    jmp Xinterrupt_common # Go to the handler

.pushsection .rodata.isr_table
.long Xisr\num
.popsection

.endm

.global Xiret
.type Xiret, @function
Xiret:
    /* call signal_is_pending
    cmp $1, %al
    je 1f
*/
2:
.global Xschedexit
.type Xschedexit,@function
Xschedexit:
    /* We'll use this bit of code as a trampoline to the new thread too */
    pop %eax
    mov %ax, %ds
    mov %ax, %es

    popall

    add $REGISTERS_UNUSED_OFF, %esp
    iretl
#if 0
1:
    mov %rsp, %rdi
    add $8, %rdi
    call handle_signal
    jmp 2b
#endif

.pushsection .text.x86_entry

/* Everything past this point is permanently mapped */

.type Xinterrupt_common,@function
.align 64
Xinterrupt_common:
    .cfi_startproc
    pushall
    cld
    mov %ds, %eax
    push %eax

    mov $KERNEL_DS, %ax
    mov %ax, %ss
    mov %ax, %ds
    mov %ax, %es

    push %esp
    mov %esp, %ebp

    call trapdisp

    mov %eax, %esp

    jmp Xiret

.cfi_endproc

.pushsection .rodata.isr_table
.balign 8
.global x86_isr_table
.type x86_isr_table, STT_OBJECT
x86_isr_table:
.popsection

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31
IRQ 0
IRQ 1
IRQ 2
IRQ 3
IRQ 4
IRQ 5
IRQ 6
IRQ 7
IRQ 8
IRQ 9
IRQ 10
IRQ 11
IRQ 12
IRQ 13
IRQ 14
IRQ 15
IRQ 16
IRQ 17
IRQ 18
IRQ 19
IRQ 20
IRQ 21
IRQ 22
IRQ 23
IRQ 24
IRQ 25
IRQ 26
IRQ 27
IRQ 28
IRQ 29
IRQ 30
IRQ 31
IRQ 32
IRQ 33
IRQ 34
IRQ 35
IRQ 36
IRQ 37
IRQ 38
IRQ 39
IRQ 40
IRQ 41
IRQ 42
IRQ 43
IRQ 44
IRQ 45
IRQ 46
IRQ 47
IRQ 48
IRQ 49
IRQ 50
IRQ 51
IRQ 52
IRQ 53
IRQ 54
IRQ 55
IRQ 56
IRQ 57
IRQ 58
IRQ 59
IRQ 60
IRQ 61
IRQ 62
IRQ 63
IRQ 64
IRQ 65
IRQ 66
IRQ 67
IRQ 68
IRQ 69
IRQ 70
IRQ 71
IRQ 72
IRQ 73
IRQ 74
IRQ 75
IRQ 76
IRQ 77
IRQ 78
IRQ 79
IRQ 80
IRQ 81
IRQ 82
IRQ 83
IRQ 84
IRQ 85
IRQ 86
IRQ 87
IRQ 88
IRQ 89
IRQ 90
IRQ 91
IRQ 92
IRQ 93
IRQ 94
IRQ 95
IRQ 96
IRQ 97
IRQ 98
IRQ 99
IRQ 100
IRQ 101
IRQ 102
IRQ 103
IRQ 104
IRQ 105
IRQ 106
IRQ 107
IRQ 108
IRQ 109
IRQ 110
IRQ 111
IRQ 112
IRQ 113
IRQ 114
IRQ 115
IRQ 116
IRQ 117
IRQ 118
IRQ 119
IRQ 120
IRQ 121
IRQ 122
IRQ 123
IRQ 124
IRQ 125
IRQ 126
IRQ 127
IRQ 128
IRQ 129
IRQ 130
IRQ 131
IRQ 132
IRQ 133
IRQ 134
IRQ 135
IRQ 136
IRQ 137
IRQ 138
IRQ 139
IRQ 140
IRQ 141
IRQ 142
IRQ 143
IRQ 144
IRQ 145
IRQ 146
IRQ 147
IRQ 148
IRQ 149
IRQ 150
IRQ 151
IRQ 152
IRQ 153
IRQ 154
IRQ 155
IRQ 156
IRQ 157
IRQ 158
IRQ 159
IRQ 160
IRQ 161
IRQ 162
IRQ 163
IRQ 164
IRQ 165
IRQ 166
IRQ 167
IRQ 168
IRQ 169
IRQ 170
IRQ 171
IRQ 172
IRQ 173
IRQ 174
IRQ 175
IRQ 176
IRQ 177
IRQ 178
IRQ 179
IRQ 180
IRQ 181
IRQ 182
IRQ 183
IRQ 184
IRQ 185
IRQ 186
IRQ 187
IRQ 188
IRQ 189
IRQ 190
IRQ 191
IRQ 192
IRQ 193
IRQ 194
IRQ 195
IRQ 196
IRQ 197
IRQ 198
IRQ 199
IRQ 200
IRQ 201
IRQ 202
IRQ 203
IRQ 204
IRQ 205
IRQ 206
IRQ 207
IRQ 208
IRQ 209
IRQ 210
IRQ 211
IRQ 212
IRQ 213
IRQ 214
IRQ 215
IRQ 216
IRQ 217
IRQ 218
IRQ 219
IRQ 220
IRQ 221
IRQ 222
IRQ 223
IRQ 224
IRQ 225

/* -- Permanent map end -- */
.popsection

.pushsection .rodata.isr_table
.global x86_isr_table_end
x86_isr_table_end:
.popsection
