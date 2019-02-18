all: libtimer.a

# Include Common Makefile
-include ${MAKE_UTILS_DIR}/Makefile.include

# Include Common Makefile
-include ${MAKE_UTILS_DIR}/Makefile.defs

INCLUDE += \
	-I$(TOP_DIR)/src/drivers/interrupts \
	-I$(TOP_DIR)/src/drivers/io \
	-I$(TOP_DIR)/src/utils/common \
	-I$(TOP_DIR)/src/utils/logger

# Generate Static library
SC_FLAG = ar -cvq

libtimer.a: $(C_OBJECTS)
	$(SC_FLAG) $(LIB_DIR)/$@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
