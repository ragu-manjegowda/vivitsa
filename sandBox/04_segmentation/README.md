# Segmentation

To understand and implement segments for code and data.

## Details:

Define descriptors for kernel code and data segments then load them when kernel
starts.

## Usage:

### To build project

```shell
	$ make
```

### To run project

```shell
	$ ./run.sh
```

### To build with debug symbols

```shell
	$ make D=-g
```

### To run project in debug mode

```shell
	$ ./run.sh -d
```

This time I switched to QEMU. If run script starts QEMU in debug mode, you need to open GDB in new terminal and connect to remote debugging on port 1234.

Writing to serial port now prints to screen, check run.sh!

#### Note: Make sure to build with debug symbols

```shell
	$ gdb or gdbtui
	$ target remote localhost:1234
	$ file build/bin/kernel.elf  (loads debugging symbols from the file)
```
