PROJECTS:=kernel
export HSD_ARCH:=$(shell scripts/hsd_arch.sh)


.PHONY: all iso clean build-prep $(SYSTEM_HEADER_PROJECTS) $(PROJECTS) \
$(SOURCE_PACKAGES) build-cleanup

export DESTDIR:=$(PWD)/sysroot
export HOST?=$(HSD_ARCH)-elf

export BUILDPKG_BIN?=$(PWD)/buildpkg/buildpkg
export BUILDPKG_BIN_PY_WRAPPER?=$(PWD)/buildpkg/buildpkg_gn_wrapper

ifneq ($(CLANG_PATH),)

export CLANG_ARGUMENTS:=--target=$(HOST)
export CLANG_BIN:=$(CLANG_PATH)/bin
export AR:=$(CLANG_BIN)/llvm-ar
export CC:=$(CLANG_BIN)/clang $(CLANG_ARGUMENTS)
export CC_BARE_PATH:=$(CLANG_BIN)/clang
export AS:=$(CC)
export CXX:=$(CLANG_BIN)/clang++ $(CLANG_ARGUMENTS)
export NM:=$(CLANG_BIN)/llvm-nm
export LD:=$(CLANG_BIN)/ld
export STRIP:=$(CLANG_BIN)/llvm-strip
export RANLIB:=$(CLANG_BIN)/llvm-ranlib
export OBJCOPY:=$(CLANG_BIN)/llvm-objcopy
export HSD_USING_CLANG:=yes

else

export AR:=$(HOST)-ar
export AS:=$(HOST)-as
export CC:=$(HOST)-gcc
export CC_BARE_PATH:=$(shell which $(CC))
export CXX:=$(HOST)-g++
export NM:=$(HOST)-nm
export LD:=$(HOST)-ld.bfd
export STRIP:=$(HOST)-strip
export RANLIB:=$(HOST)-ranlib
export OBJCOPY:=$(HOST)-objcopy

endif

export PREFIX:=/usr
export EXEC_PREFIX:=$(PREFIX)
export BOOTDIR:=/boot
export LIBDIR:=$(EXEC_PREFIX)/lib
export INCLUDEDIR:=$(PREFIX)/include
export BINDIR:=$(PREFIX)/bin
export MANDIR:=/usr/share/man
export PKGDIR:=/pkg

ifeq ($(HSD_ARCH), riscv64)
ifeq ($(HSD_USING_CLANG), yes)
export EXTRA_CFLAGS:=-mno-relax
endif
endif

export CFLAGS?=-O2 -g $(EXTRA_CFLAGS)
export CPPFLAGS:=

export SYSROOT=$(PWD)/sysroot

# Configure the cross-compiler to use the desired system root.
export CXX:=$(CXX) --sysroot=$(PWD)/sysroot
export CC:=$(CC) --sysroot=$(PWD)/sysroot

all: iso

clean:
	for module in $(ALL_MODULES); do $(MAKE) -C $$module clean; done
	make -C usr/sys clean
	rm -rf sysroot
	rm -rf initrd.tar.*

build-prep:
	mkdir -p sysroot

install-packages: $(PROJECTS)

kernel: install-headers
	rm -f kernel/clang-tidy.out
	$(MAKE) -C usr/sys install

install-headers: build-prep
#	$(MAKE) -C kernel install-headers

build-srcpackages: $(SOURCE_PACKAGES)

build-usystem: kernel

build-cleanup: build-usystem 
	$(NM) $(SYSROOT)/boot/genunix > $(SYSROOT)/boot/genunix.map

fullbuild: build-cleanup

iso: fullbuild
	scripts/iso.sh

liveiso: fullbuild
	scripts/iso.sh scripts/livecd-initrd.sh

fullbuild-plus-initrd: fullbuild
	SYSTEM_ROOT=$(SYSROOT) scripts/geninitrd --compression-method none scripts/default-initrd.sh

fullbuild-plus-full-initrd: fullbuild
	SYSTEM_ROOT=$(SYSROOT) scripts/geninitrd --compression-method none scripts/livecd-initrd.sh

qemu: iso
	qemu-system-i386 -s -cdrom hsd.iso --enable-kvm -no-shutdown -no-reboot
