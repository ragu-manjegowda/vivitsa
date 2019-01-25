all: libio.a

# Include Common Makefile
-include ${MAKE_UTILS_DIR}/Makefile.include

# Include Common Makefile
-include ${MAKE_UTILS_DIR}/Makefile.defs

AS = nasm
ASFLAGS = -f elf

# Generate Static library
SC_FLAG = ar -cvq

libio.a: $(S_OBJECTS)
	$(SC_FLAG) $(LIB_DIR)/$@ $^

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf *.o *.so
