#OBJECTS = loader.o kmain.o

LDFLAG += -T link.ld -melf_i386

all: kernel.elf

INCLUDE += \
	-I. \
	-I$(TOP_DIR)/src/drivers/frame_buffer

# Include Common Makefile
-include ${MAKE_UTILS_DIR}/Makefile.include

# Include Common Makefile
-include ${MAKE_UTILS_DIR}/Makefile.defs

C_SOURCES += $(TOP_DIR)/src/drivers/frame_buffer/frame_buffer.c

S_SOURCES += $(TOP_DIR)/src/drivers/frame_buffer/io.s

kernel.elf: $(C_OBJECTS) $(S_OBJECTS)
	@echo $(C_OBJECTS) $(S_OBJECTS)
	ld $(LDFLAG) $(C_OBJECTS) $(S_OBJECTS) -o $(BIN_DIR)/$@

%.o: %.c
		$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

%.o: %.s
		$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf *.o
