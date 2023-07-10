/* SPDX-License-Identifier: GPL-2.0-ONLY */
#define CONST1 (0x01010101UL)
#define CONST2 (0x80808080UL)

#define WORD_SIZE     (sizeof(unsigned long))
#define ALIGNED(x, y) !((unsigned long) x & (y - 1))
#define HASZERO(v)    (((v) -CONST1) & ~(v) &CONST2)

typedef unsigned long __attribute__((__may_alias__)) word_t;

unsigned long strlen(const char *s)
{
    const char *start = s;
    const word_t *ptr;

    while (!ALIGNED(s, WORD_SIZE))
    {
        if (!*s)
            return s - start;
        s++;
    }

    ptr = (const word_t *) s;

    for (; !HASZERO(*ptr); ptr++)
        ;

    s = (const char *) ptr;
    for (; *s; s++)
        ;
    return s - start;
}
