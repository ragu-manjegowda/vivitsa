#!/usr/bin/env bash

if [ "$#" -eq 1 ]; then
  if [ $1 = "-d" ]; then
    debug="-S";
    echo "Running in debug mode"
  else
    echo " './run.sh -d' to run in debug mode"
    exit 1
  fi
fi

cp build/bin/kernel.elf iso/boot/kernel.elf

genisoimage -R                  \
-b boot/grub/stage2_eltorito    \
-no-emul-boot                   \
-boot-load-size 4               \
-A os                           \
-input-charset utf8             \
-quiet                          \
-boot-info-table                \
-o vivitsa.iso                  \
iso

qemu-system-i386 -nographic -cdrom vivitsa.iso -m 4G -s -serial mon:stdio $debug > qemu.log 2>&1 &
