# Loading Program 

Load program from disk.

## Details:

Understand loading program we want to execute into memory. 

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
