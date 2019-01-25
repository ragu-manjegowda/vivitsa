# Understanding BIOS, GRUB, OSGEN and Bochs 

## Details:

To understand booting sequence in an operating system wrote a simple kernel 
program that write 0xCAFEBABE to EAX register and keep spinning in infinite loop

Used GNU GRUB off the shelf as bootloader, linker has the information to load 
lernel at 1MB. And align all the segments in multiple of 4.

## Usage:

### To build project

```shell
   $ ./build.sh
```
### To run project
   
```shell
	$ ./run.sh
```

This will fire up the Bochs emulator, press C to continue. OS now boots up. 
oxCAFEBABE is stored in EAX register. Then Kernel goes to infinte loop, press q
in shell which quits Bochs. Grep bochslog.txt to see value written to EAX 
register. 
