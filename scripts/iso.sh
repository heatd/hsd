#!/bin/sh
set -e

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

INITRD_GEN_SCRIPT=$1

if [ "$INITRD_GEN_SCRIPT" = "" ]; then
    INITRD_GEN_SCRIPT="scripts/default-initrd.sh"
fi

#SYSTEM_ROOT=$DESTDIR scripts/geninitrd "$INITRD_GEN_SCRIPT"

cp sysroot/boot/genunix isodir/boot/genunix
#INITRD_NAME=initrd.tar.zst
#cp $INITRD_NAME isodir/boot

echo "Compressing kernel"
xz -9 -e -f isodir/boot/genunix

cat > isodir/boot/grub/grub.cfg << EOF
set default="0"
set timeout=5
menuentry "HSD" {
    loadfont unicode
    insmod all_video
    insmod xzio
    insmod gfxterm
    terminal_output gfxterm
    set gfxpayload=1024x768x32

    echo "Loading the HSD kernel"
    multiboot2 /boot/genunix.xz --root=/dev/sda1
    echo "Loading the initrd"
    #module2 /boot/${INITRD_NAME}

    boot
}
EOF
grub-mkrescue -o hsd.iso isodir # Change this acording to your distro/OS.
