
# Vivitsa
  My operating system - from scratch!!!

I am naming this OS after Sanskrit word Vivitsa meaning "desire of knowledge".

This Operating system is targeted to run on x86 architecture. Initially I used
[Bochs emulator](https://en.wikipedia.org/wiki/Bochs) to run and test my OS
while developing, later switched to
[QEMU x86 Emulator](https://en.wikipedia.org/wiki/QEMU) as I found it handy.

To build, run and test please use my
[docker image](https://hub.docker.com/r/ragumanjegowda/docker) by running

```shell
	$ ./run.sh
```
which has all the dependencies installed on it. I recommend running this image
on Ubuntu if you want to run emulator inside docker.

Note: When you are inside docker, host PCs HOME directory is mounted under /mnt

### To run, debug and to know more please refer README.md in sub-folders.

References:
1. http://www.jamesmolloy.co.uk/tutorial_html/
2. https://littleosbook.github.io/
3. https://wiki.osdev.org/Getting_Started
4. https://github.com/tuhdo/os01
5. http://mikeos.sourceforge.net/write-your-own-os.html
