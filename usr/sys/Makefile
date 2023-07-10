ifeq ($(CONFIG_CC_OPTIMIZE_SIZE), y)
CFLAGS?=-Os -g
else
CFLAGS?=-O2 -g
endif

CPPFLAGS?=
LDFLAGS?=
LIBS?=

DESTDIR?=
PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
BOOTDIR?=$(EXEC_PREFIX)/boot
INCLUDEDIR?=$(PREFIX)/usr/include
LIBDIR?=$(EXEC_PREFIX)/usr/lib

KERNEL_TAGS?=

CFLAGS:=$(CFLAGS) -MMD -ffreestanding -fbuiltin -Wall -Wextra \
-Wno-unused-parameter -Wno-unused-function \
-Werror -Wno-error=frame-address -Wno-address-of-packed-member \
-Wno-error=invalid-offsetof -fno-sanitize=pointer-overflow -fvisibility=protected \
-Wno-missing-field-initializers -Wstrict-aliasing -fno-unwind-tables -fno-asynchronous-unwind-tables

ifneq ($(ONYX_USING_CLANG), yes)
# TODO: Find suitable alternatives for clang
CFLAGS:=$(CFLAGS) -Wno-format-truncation -Wshadow-compatible-local

# Note: We need to define _GLIBCXX_INCLUDE_NEXT_C_HEADERS so the shenanigans in the
# compiler (in this case, only GCC)'s stdlib.h C++ wrappers get bypassed. If we let stdlib.h include cstdlib,
# like it wants to when (__cplusplus || _GLIBCXX_INCLUDE_NEXT_C_HEADERS) is false, it inlines a bunch of
# functions with __builtin_*(), which in many cases have double and float arguments and GCC breaks due to
# floating point being disabled

CPPFLAGS:=$(CPPFLAGS) -D_GLIBCXX_INCLUDE_NEXT_C_HEADERS
else

# Explanation for these options:
# -Wno-null-pointer-arithmetic: ACPICA triggers this with ACPI_ROOT_OBJECT
# -Wno-unknown-attributes: as of llvm 11.0.0, it doesn't recognize [[likely]] and [[unlikely]], which we do
#     use quite often.
# -Wno-missing-braces: Seems broken? At least it started spitting out weird ones for IPv6 code(IN6ADDR_ALL_NODES)
#	  in onyx/net/ipv6.h
CFLAGS:=$(CFLAGS) -Wno-null-pointer-arithmetic -Wno-unknown-attributes -Wno-error=unused-private-field \
	-Wno-missing-braces -Wno-c99-designator
endif

# NOTE: I don't think pointer-overflow works with kernel pointers

CPPFLAGS:=$(CPPFLAGS) -D__KERNEL__  -Iinclude
LDFLAGS:=$(LDFLAGS) -fkeep-inline-functions -Wl,--build-id=none -nostdlib

OBJS:=stand/boot.o stand/init.o stand/mem.o sys/pagealloc.o stand/trap.o stand/locore.o stand/gdt.o stand/serial.o\
      stand/pic.o sys/printf.o sys/puts.o sys/str.o sys/malloc.o stand/panic.o stand/pit.o

%.o: %.c $(GENERATED_HEADERS)
	@$(CC) -c $< -o $@ -std=gnu89 $(CFLAGS) $(CPPFLAGS) $(C_ONLY_CFLAGS) -Wno-error=deprecated-declarations

%.o: %.S $(GENERATED_HEADERS)
	@echo [AS] $<
	@$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

genunix: $(OBJS)
	$(CC) -Tstand/kern.lds $(CFLAGS) $(LDFLAGS) $(OBJS) -o $@

install: genunix
	mkdir -p $(DESTDIR)$(BOOTDIR)
	cp genunix $(DESTDIR)$(BOOTDIR)

clean:
	rm -f genunix $(OBJS)
