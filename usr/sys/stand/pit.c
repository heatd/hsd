/*
 * Copyright (c) 2016 - 2021 Pedro Falcato
 * This file is part of Onyx, and is released under the terms of the GPLv2 License
 * check LICENSE at the root directory for more information
 *
 * SPDX-License-Identifier: GPL-2.0-ONLY
 */

#define INT_DIV_ROUND_CLOSEST(x, y) (((x) + ((y) / 2)) / (y))

#include <stand/port.h>

#define PIT_CHANNEL0_DATA 0x40
#define PIT_CHANNEL1_DATA 0x41
#define PIT_CHANNEL2_DATA 0x42
#define PIT_COMMAND_REG   0x43

#define PIT_COMMAND_CHANNEL(x)         ((x) << 6)
#define PIT_COMMAND_ACCESS_LATCH_COUNT (0)
#define PIT_COMMAND_ACCESS_LOBYTE_ONLY (1 << 4)
#define PIT_COMMAND_ACCESS_HIBYTE_ONLY (1 << 5)
#define PIT_COMMAND_ACCESS_LOHIBYTE \
    (PIT_COMMAND_ACCESS_LOBYTE_ONLY | PIT_COMMAND_ACCESS_HIBYTE_ONLY)
#define PIT_COMMAND_MODE(x)     ((x) << 1)
#define PIT_COMMAND_BINARY_MODE (0 << 0)
#define PIT_COMMAND_BCD_MODE    (1 << 0)

#define PIT_READBACK_MUST_ONE          ((1 << 7) | (1 << 6))
#define PIT_READBACK_DONT_LATCH_COUNT  (1 << 5)
#define PIT_READBACK_DONT_LATCH_STATUS (1 << 4)
#define PIT_READBACK_CHANNEL2          (1 << 3)
#define PIT_READBACK_CHANNEL1          (1 << 2)
#define PIT_READBACK_CHANNEL0          (1 << 1)

#define PIT_FREQUENCY 1193180

#define PIT_STATUS_OUTPUT_HIGH (1 << 7)

void pitinit(unsigned frequency)
{
    unsigned short divisor = INT_DIV_ROUND_CLOSEST(PIT_FREQUENCY, frequency);

    unsigned char command = PIT_COMMAND_CHANNEL(0) |
        PIT_COMMAND_ACCESS_LOHIBYTE | PIT_COMMAND_MODE(3) | PIT_COMMAND_BINARY_MODE;

    /* Send the command */
    outb(PIT_COMMAND_REG, command);

    /* Set the divisor */
    outb(PIT_CHANNEL0_DATA, divisor & 0xFF);

    outb(PIT_CHANNEL0_DATA, divisor >> 8);
}

void pit_send_readback(unsigned char channels, int count, int status)
{
    unsigned char command = 0;
    command |= PIT_READBACK_MUST_ONE;
    if (!count)
        command |= PIT_READBACK_DONT_LATCH_COUNT;
    if (!status)
        command |= PIT_READBACK_DONT_LATCH_STATUS;

    /* Mask the channels and OR them in */
    command |= (channels & 0x3);

    outb(PIT_COMMAND_REG, command);
}

void pit_wait_for_oneshot(void)
{
    unsigned char status = 0;
    do
    {
        /* Send a readback command to read the status of channel 0 */
        pit_send_readback(PIT_READBACK_CHANNEL0, 0, 1);
        status = inb(PIT_CHANNEL0_DATA);
    } while (!(status & PIT_STATUS_OUTPUT_HIGH));
}

void pit_stop(void)
{
    outb(PIT_COMMAND_REG, 0x38);
}
