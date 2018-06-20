#!/usr/bin/env bash

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

qemu-system-i386 -cdrom vivitsa.iso -s -serial stdio -S
