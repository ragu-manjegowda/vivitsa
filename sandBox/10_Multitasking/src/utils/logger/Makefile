all: liblogger.a

# Include Common Makefile
-include ${MAKE_UTILS_DIR}/Makefile.include

C_SOURCES += $(TOP_DIR)/src/drivers/frame_buffer/frame_buffer.c \
	$(TOP_DIR)/src/drivers/serial_port/serial_port.c

# Include Common Makefile
-include ${MAKE_UTILS_DIR}/Makefile.defs

INCLUDE += \
	-I$(TOP_DIR)/src/drivers/frame_buffer \
	-I$(TOP_DIR)/src/drivers/io \
	-I$(TOP_DIR)/src/drivers/serial_port \
	-I$(TOP_DIR)/src/drivers/timer \
	-I$(TOP_DIR)/src/utils/common

# Generate Static library
SC_FLAG = ar -cvq

liblogger.a: $(C_OBJECTS) $(S_OBJECTS)
	$(SC_FLAG) $(LIB_DIR)/$@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@
