#!/usr/bin/env bash

cp build/bin/kernel.elf iso/boot/kernel.elf
cp build/etc/bochsrc.txt .

genisoimage -R                  \
-b boot/grub/stage2_eltorito    \
-no-emul-boot                   \
-boot-load-size 4               \
-A os                           \
-input-charset utf8             \
-quiet                          \
-boot-info-table                \
-o os.iso                       \
iso

bochs -f bochsrc.txt -q
