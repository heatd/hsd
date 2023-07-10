/* SPDX-License-Identifier: GPL-2.0-ONLY */
#include <out.h>
#include <stdarg.h>
#include <str.h>

struct stream
{
    int (*out)(const char *c, int len, struct stream *);
};

static
int strout(const char *c, int len, struct stream *stream)
{
    int ret = 0;

    while (len-->0)
        outc(*c++), ret++;
    return ret;
}

static struct stream outstr =
{
    strout 
};

enum integer_type
{
    ITYPE_HH,
    ITYPE_H,
    ITYPE_INT,
    ITYPE_L,
    ITYPE_LL
};

#define F_PALT       1
#define F_PZERO      2
#define F_PLEFT      4
#define F_PBLANK     8
#define F_PSIGN      16
#define F_PUNSIGNED  32
#define F_PHEX       64
#define F_PUPPERHEX  128
#define F_POCTAL     256

static const char *digits = "0123456789abcdefghijklmnop";
static const char *DIGITS = "0123456789ABCDEFGHIJKLMNOP";

/* print an int */
static
int pint(struct stream *stream, unsigned long val, int width, short flags)
{
#define TBUFMAX 128
#define PUTBUF(c) buf[TBUFMAX - 1 - (buflen++)] = c

    char buf[TBUFMAX];
    int buflen = 0;
    const char *d = digits;
    unsigned char base = 10;
    int sign = 0;
    char wazero = 0;

    if (flags & (F_PHEX | F_PUPPERHEX))
        base = 16;
    else if (flags & F_POCTAL)
    {
        if (val == 0)
            wazero = 1;
        base = 8;
    }
    
    if (flags & F_PUPPERHEX)
        d = DIGITS;
    
    if (!(flags & F_PUNSIGNED))
    {
        if ((long) val < 0)
        {
            val = -val;
            sign = 1;
        }
    }
    
    while (val)
    {
        unsigned digit = val % base;
        val /= base;

        PUTBUF(d[digit]);
    }

    if (flags & F_PALT)
    {
        if (flags & (F_PHEX | F_PUPPERHEX))
        {
            PUTBUF('x');
            PUTBUF('0');
        }
        else if (flags & F_POCTAL)
        {
            if (!wazero)
                PUTBUF('0');
        }
    }

    if (!(flags & F_PUNSIGNED))
    {
        if (sign)
        {
            PUTBUF('-');
        }
        else
        {
            if (flags & F_PSIGN)
                PUTBUF('+');
            else if (flags & F_PBLANK)
                PUTBUF(' ');
        }
    }

    return stream->out(buf + TBUFMAX - buflen, buflen, stream);
}
#undef PUTBUF
#undef TBUFMAX


int vfprintf(struct stream *stream, const char *s, va_list va)
{
    int perc = 0, ret = 0, sret;
    int fwidth = 0;
    enum integer_type itype = ITYPE_INT;
    short flags = 0;
    unsigned long val = 0;
    char c;

    while ((c = *s++))
    {
        if (perc)
            goto special;
        if (c == '%')
        {
            perc = 1;
            continue;
        }

putc:
        sret = stream->out(&c, 1, stream);

        if (sret < 0)
        {
            ret = sret;
            goto out;
        }

        ret += sret;

        continue;

special:
        if (c == '%')
        {
            perc = 0;
            goto putc;
        }
        
        /* Handle flags */
        switch (c)
        {
            case '0':
                flags |= F_PZERO;
                continue;
            case ' ':
                flags |= F_PBLANK;
                continue;
            case '#':
                flags |= F_PALT;
                continue;
            case '+':
                flags |= F_PSIGN;
                continue;
            case '-':
                flags |= F_PLEFT;
                continue;
        }

        /* XXX missing min precision, field width */
        /* handle the type */
        switch (c)
        {
            case 'h':
                itype = itype == ITYPE_H ? ITYPE_HH : ITYPE_H;
                continue;
            case 'l':
                itype = itype == ITYPE_L ? ITYPE_LL : ITYPE_L;
                continue;
        }

        /* handle the actual specifier */
        switch (c)
        {
            /* XXX todo more */
            case 'u':
                flags |= F_PUNSIGNED;
                goto printint;
            case 'x':
                flags |= F_PUNSIGNED | F_PHEX;
                goto printint;
            case 'X':
                flags |= F_PUNSIGNED | F_PUPPERHEX;
                goto printint;
            case 'o':
                flags |= F_POCTAL | F_PUNSIGNED;
                goto printint;
            case 'p':
                /* make %p %#lx */
                flags |= F_PALT | F_PUNSIGNED | F_PHEX;
                itype = ITYPE_L;
                goto printint;
            case 'd':
                goto printint;
            case 's': {
                const char *str = va_arg(va, const char *);
                sret = stream->out(str, strlen(str), stream);
                if (sret < 0)
                {
                    ret = sret;
                    goto out;
                }
                ret += sret;
                goto reset;
            }
        }

        /* XXX unhandled? weird control flow... */
        continue;
printint:

        switch (itype)
        {
            case ITYPE_H:
                val = (unsigned char) va_arg(va, unsigned int);
                break;
            case ITYPE_HH:
                val = (unsigned short) va_arg(va, unsigned int);
                break;
            case ITYPE_INT:
                val = va_arg(va, unsigned int);
                break;
            case ITYPE_L:
                val = va_arg(va, unsigned long);
                break;
            case ITYPE_LL:
                /* TODO: long long is borked cuz of libgcc */
                val = va_arg(va, unsigned long long);
                break;
        }

        sret = pint(stream, val, fwidth, flags);

        if (sret < 0)
            goto out;
        ret += sret;

reset:
        flags = 0;
        fwidth = 0;
        itype = ITYPE_INT;
        perc = 0;
    }
out:
    return ret;
}

int
printf(const char *str, ...)
{
    va_list list;
    int ret;

    va_start(list, str);
    ret = vfprintf(&outstr, str, list);
    va_end(list);

    return ret;
}
