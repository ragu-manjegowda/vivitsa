OBJECTS = io.o frame_buffer.o

all: frame_buffer.so

# Include Common Makefile
-include ${MAKE_UTILS_DIR}/Makefile.include

LDFLAGS += -shared
SO_FLAG = -c -fpic
AS = nasm
ASFLAGS = -f elf

frame_buffer.so: $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $(LIB_DIR)/$@


%.o: %.c
	$(CC) $(CFLAGS) $(SO_FLAG) $<

%.o: %.s
	$(AS) $(ASFLAGS) $<

clean:
	rm -rf *.o *.so
