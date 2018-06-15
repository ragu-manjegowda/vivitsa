LDFLAG += -T link.ld -melf_i386
LIBS = -lframe_buffer

all: kernel.elf

# Include Common Makefile
-include ${MAKE_UTILS_DIR}/Makefile.include

# Include Common Makefile
-include ${MAKE_UTILS_DIR}/Makefile.defs

INCLUDE += \
	-I. \
	-I$(TOP_DIR)/src/drivers/frame_buffer

kernel.elf: $(C_OBJECTS) $(S_OBJECTS)
	@echo $(C_OBJECTS) $(S_OBJECTS)
	ld $(LDFLAG) $(LDFLAGS) $(C_OBJECTS) $(S_OBJECTS) $(LIBS) -o $(BIN_DIR)/$@

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf *.o
