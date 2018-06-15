#OBJECTS = io.o frame_buffer.o

all: frame_buffer.so

# Include Common Makefile
-include ${MAKE_UTILS_DIR}/Makefile.include

# Include Common Makefile
-include ${MAKE_UTILS_DIR}/Makefile.defs

LDFLAGS += -shared
SO_FLAG = -c -fpic
AS = nasm
ASFLAGS = -f elf

INCLUDE += \
	-I. \

frame_buffer.so: $(C_OBJECTS) $(S_OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(C_OBJECTS) $(S_OBJECTS) -o $(LIB_DIR)/$@

%.o: %.c
	$(CC) $(CFLAGS) $(SO_FLAG) $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf *.o *.so
