#!/bin/bash

# Compile the assembly code
nasm -f elf32 loader.s

# Generate executable from object code compiled previously and link with linker 
ld -T link.ld -melf_i386 loader.o -o kernel.elf

# Generate os.iso from the above kernel elf file
genisoimage -R                              \
						-b boot/grub/stage2_eltorito    \
						-no-emul-boot                   \
						-boot-load-size 4               \
						-A os                           \
						-input-charset utf8             \
						-quiet                          \
						-boot-info-table                \
						-o os.iso                       \
						iso
