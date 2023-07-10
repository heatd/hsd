/*
 * Copyright (c) 2016 - 2023 Pedro Falcato
 * This file is part of Onyx, and is released under the terms of the GPLv2 License
 * check LICENSE at the root directory for more information
 *
 * SPDX-License-Identifier: GPL-2.0-ONLY
 */

ENTRY(i386_unix_main)

VIRT_BASE = 0xC0000000;
PHYS_BASE = 0x100000;

SECTIONS
{
	. = PHYS_BASE;
	kernel_phys_start = .;
	kernel_start = VIRT_BASE + .;
	_text_start = . + VIRT_BASE;

	. += VIRT_BASE;

	.text : AT(ADDR(.text) - VIRT_BASE)
	{
        *(.text.head)
		*(.text*)
		*(.gnu.linkonce.t*)
		*(.init)
		*(.fini)
		*(.jcr)
		*(.dtors)
		*(.ctors)
	} : text
	
	_ro_start = .;

	.rodata ALIGN(0x1000) : AT(ADDR(.rodata) - VIRT_BASE)
	{
		PROVIDE_HIDDEN(__init_array_start = .);
		*(.init_array*)
		PROVIDE_HIDDEN(__init_array_end = .);
		PROVIDE_HIDDEN(__fini_array_start = .);
		*(.fini_array*)
		PROVIDE_HIDDEN(__fini_array_end = .);
		*(.rodata*)
		*(.gnu.linkonce.r*)
	} : rodata

	.note.gnu.build-id :
	{
        PROVIDE_HIDDEN(__build_id_note_start = .);
        *(.note.gnu.build-id)
        PROVIDE_HIDDEN(__build_id_note_end = .);
    } :rodata :note

	_ro_end = .;
	. = ALIGN(0x1000);

	_text_end = .;
	_data_start = .;

	.data ALIGN(0x1000) : AT(ADDR(.data) - VIRT_BASE)
	{
		*(.data*)
		*(.gnu.linkonce.d*)
	} : data

	. = ALIGN(0x1000);

	_bss_start = .;
	.bss : AT(ADDR(.bss) - VIRT_BASE)
	{
		*(COMMON*)
		*(.bss*)
		*(.gnu.linkonce.b*)
	} : data
	_bss_end = .;
	_bss_size = _bss_end - _bss_start;

	. = ALIGN(0x1000);
	_data_end = .;
	_data_size = _data_end - _data_start;

	__pecoff_data_size = _bss_start - _data_start;

	. = ALIGN(0x1000);
	_vdso_sect_end = .;
	kernel_end = .;
	kernel_phys_end = . - VIRT_BASE;
	PROVIDE_HIDDEN(__kernel_size = kernel_end - kernel_start);
}

PHDRS
{
	boot PT_LOAD FLAGS(5);
	text PT_LOAD FLAGS(5);
	rodata PT_LOAD FLAGS(4);
	data PT_LOAD FLAGS(6);
	note PT_NOTE FLAGS(0);
}
