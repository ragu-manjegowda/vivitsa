# Paging

Enable Paging.

## Details:

Understand and implement paging. Map kernel to higher memory make room for
programs that run at lesser privilege level.

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
