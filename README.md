
# Vivitsa
  My operating system - from scratch!!!

Writing an operating operating system from scratch is fun. I am naming this OS after Sanskrit word Vivitsa meaning "desire of knowledge" which is exactly why I started this.


I have named the folders starting with number followed by heading so that it is
easy to maintain chronology.

This Operating system is targeted to run on x86 architecture, while developing I used [Bochs emulator](https://en.wikipedia.org/wiki/Bochs) initially. But, later I found [QEMU x86 Emulator](https://en.wikipedia.org/wiki/QEMU) more handy.

To build, run and test please use my
[docker image](https://github.com/ragu-manjegowda/ragu-docker) by simply running

```shell
	$ docker pull ragumanjegowda/docker
```
which has all the dependencies installed on it. I recommend running this image
on Ubuntu if you want to run emulator inside docker.

### To run, debug and to know more please refer README.md in sub-folders.

References:
1. http://www.jamesmolloy.co.uk/tutorial_html/
2. https://littleosbook.github.io/
3. https://wiki.osdev.org/Getting_Started
4. https://github.com/tuhdo/os01
5. http://mikeos.sourceforge.net/write-your-own-os.html
