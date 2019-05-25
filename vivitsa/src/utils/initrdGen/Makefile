.PHONY: all clean install

all: initrdGen

clean: $(SUBDIRS) rmOBJS
	for subdir in $(SUBDIRS) ; do \
		make -C $$subdir clean ; \
	done

install:
	echo "Install not supported, please copy manually"

rmOBJS:
	rm -rf .DS_Store *.o *.so *.a *.out initrdGen initrd

CC = gcc

C_SOURCES += $(wildcard *.c)
C_OBJECTS += $(C_SOURCES:.c=.o)

initrdGen : $(C_OBJECTS)
	$(CC) $^ -o $@

%.o: %.c
	$(CC) -c $< -o $@
