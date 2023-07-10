/* SPDX-License-Identifier: GPL-2.0-ONLY */
struct gdte
{
    unsigned short gde_limitlo;
    unsigned short gde_baselo;
    unsigned char gde_basemid;
    unsigned char gde_type;
    unsigned char gde_flags;
    unsigned char gde_basehi;
} __attribute__((packed));

#define GDT_S (1 << 4)
#define GDT_DPL(dpl) ((dpl) << 5)
#define GDT_P (1 << 7)
#define GDT_DB (1 << 6)
#define GDT_G (1 << 7)

#define GDT_CS_TYPE (0xa)
#define GDT_DS_TYPE (0x2)

#define GDT_FLAGS(limit, flags) (limit | flags)

static struct gdte gdt[] = {{0},
    {0xffff, 0, 0, GDT_CS_TYPE | GDT_S | GDT_P | GDT_DPL(0), GDT_FLAGS(0xf, GDT_DB | GDT_G), 0}, /* 0x8 - 32-bit kernel cs */
    {0xffff, 0, 0, GDT_DS_TYPE | GDT_S | GDT_P | GDT_DPL(0), GDT_FLAGS(0xf, GDT_DB | GDT_G), 0}, /* 0x10 - 32-bit kernel ds */
    {0xffff, 0, 0, GDT_CS_TYPE | GDT_S | GDT_P | GDT_DPL(3), GDT_FLAGS(0xf, GDT_DB | GDT_G), 0}, /* 0x18 - 32-bit user cs */
    {0xffff, 0, 0, GDT_DS_TYPE | GDT_S | GDT_P | GDT_DPL(3), GDT_FLAGS(0xf, GDT_DB | GDT_G), 0}  /* 0x20 - 32-bit user ds */
};

void loadgdt(unsigned short limit, void *);

void initgdt(void)
{
    loadgdt(sizeof(gdt) - 1, gdt);
}
