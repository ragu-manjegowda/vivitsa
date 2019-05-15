# Paging

Enable Paging.

## Details:

Understand and implement paging. Map kernel to higher memory make room for
programs that run at lesser privilege level.

Note: Top most commit doesn't run as it has experimental code to relocate kernel
to 3GB virtual address.

## Usage:

### To build project

```shell
	# -s for make quite
	$ ./build.sh -s
```

### To run project

```shell
	$ ./run.sh
```

### To build with debug symbols

```shell
	# -s for make quite, -g for debug
	$ ./build.sh -s -g
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
