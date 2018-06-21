# Interrupts

To understand and implement interrupt service routines.

## Details:

Define ISRs, load them to processor and handle them appropriately when it occurs.

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

#### Note: Make sure to build with debug symbols

```shell
	$ gdb or gdbtui
	$ target remote localhost:1234
	$ file build/bin/kernel.elf  (loads debugging symbols from the file)
```
