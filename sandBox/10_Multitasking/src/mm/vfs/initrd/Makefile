all: libinitrd.a

# Include Common Makefile
-include ${MAKE_UTILS_DIR}/Makefile.include

# Include Common Makefile
-include ${MAKE_UTILS_DIR}/Makefile.defs

INCLUDE += \
	-I$(TOP_DIR)/src/mm/heap \
	-I$(TOP_DIR)/src/mm/vfs/fs \
	-I$(TOP_DIR)/src/utils/common

# Generate Static library
SC_FLAG = ar -cvq

libinitrd.a: $(C_OBJECTS)
	$(SC_FLAG) $(LIB_DIR)/$@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
