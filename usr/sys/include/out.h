/* SPDX-License-Identifier: GPL-2.0-ONLY */
#ifndef _OUT_H
#define _OUT_H

/* output a character to the console */
void outc(char c);

/* output a string to the console */
int out(const char *s);

int
printf(const char *str, ...);

/* output a string to the console */
int
puts(const char *s);

/* panic the system */
void
panic(const char *str);

#endif
