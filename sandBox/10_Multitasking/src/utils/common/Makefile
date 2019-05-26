all: libcommon.a

# Include Common Makefile
-include ${MAKE_UTILS_DIR}/Makefile.include

# Include Common Makefile
-include ${MAKE_UTILS_DIR}/Makefile.defs

# Generate Static library
SC_FLAG = ar -cvq

libcommon.a: $(C_OBJECTS) $(S_OBJECTS)
	$(SC_FLAG) $(LIB_DIR)/$@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@
