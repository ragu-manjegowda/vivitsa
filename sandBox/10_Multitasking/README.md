# Virtual File System

Implement Virtual File System.

## Details:

Implement File system and initrd. Which are required to enable vfs.

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
