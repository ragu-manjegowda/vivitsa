LDFLAG += -T link.ld -melf_i386
LIBS = -llogger -lgdt -lidt -lio -lcommon -ltimer -lkb -lpaging -lkheap \
	-lorderedarray -lfs -linitrd -lsched

all: kernel.elf

# Include Common Makefile
-include ${MAKE_UTILS_DIR}/Makefile.include

# Include Common Makefile
-include ${MAKE_UTILS_DIR}/Makefile.defs

INCLUDE += \
	-I$(TOP_DIR)/src/drivers/interrupts \
	-I$(TOP_DIR)/src/drivers/keyboard \
	-I$(TOP_DIR)/src/drivers/serial_port \
	-I$(TOP_DIR)/src/drivers/timer \
	-I$(TOP_DIR)/src/mm/heap \
	-I$(TOP_DIR)/src/mm/paging \
	-I$(TOP_DIR)/src/mm/sched \
	-I$(TOP_DIR)/src/mm/segmentation \
	-I$(TOP_DIR)/src/mm/vfs/fs \
	-I$(TOP_DIR)/src/mm/vfs/initrd \
	-I$(TOP_DIR)/src/tests \
	-I$(TOP_DIR)/src/utils/common \
	-I$(TOP_DIR)/src/utils/logger

kernel.elf: $(C_OBJECTS) $(S_OBJECTS)
	ld $(LDFLAG) $(LDFLAGS) $(C_OBJECTS) $(S_OBJECTS) $(LIBS) -o $(BIN_DIR)/$@

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@
