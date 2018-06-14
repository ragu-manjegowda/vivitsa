# Calling C function from Assembly

To understand how to call minimal C function not including any libraries,
from assembly. Write a Makefile to compile and run. 

## Details:

C function in kmain.c is called from loader.s assembly code. Learnt how to 
pass arguments to calle functions and to receive back the return value in
calling function.

## Usage:

### To build project

```shell
   $ make
```
or

```shell
	$ make all
```

### To run project
   
```shell
	$ make run
```

This will fire up the Bochs emulator, press C to continue. OS now boots up. 
sum of arguments is returned from C function that is called from assembly. And
it is stored in EAX register. Then Kernel goes to infinte loop, press q in shell
which quits Bochs. Grep bochslog.txt to see value written to EAX register. 
