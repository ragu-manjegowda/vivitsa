## To generate initrd image that can be loaded into kernel as module

## Usage:

  Files that needs to loaded as initrd should be passed as argument at odd
index and the desired/new filename within OS should be specified at even index.

TODO: Support for directories and multi level files (files within directories)

### To compile

```shell
	# -s for make quite
	$ make -s
```

### To generate module

```shell
	$ ./initrdGen "i/p file name 1" "o/p file name 1" "i/p file name 2" \
                "o/p file name 2" .......... "i/p file name N" "o/p file name N"
```

### Example

```shell
	$ ./initrdGen file1.txt file1.txt file2.txt file2.txt
```
