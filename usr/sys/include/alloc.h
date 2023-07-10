/* SPDX-License-Identifier: GPL-2.0-ONLY */
#ifndef _ALLOC_H
#define _ALLOC_H

#include <stddef.h>

void *malloc(size_t len);
void free(void *ptr);
void *realloc(void *oldp, size_t newsz);
void *calloc(size_t nelem, size_t sz);

#endif
