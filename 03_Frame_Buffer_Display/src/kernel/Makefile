OBJECTS = loader.o kmain.o

LDFLAG += -T link.ld -melf_i386

all: kernel.elf

# Include Common Makefile
-include ${MAKE_UTILS_DIR}/Makefile.include

kernel.elf: $(OBJECTS)
		ld $(LDFLAG) $(OBJECTS) -o $(BIN_DIR)/$@

%.o: %.c
		$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
		$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf *.o
