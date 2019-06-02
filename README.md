
[![Build Status](https://travis-ci.org/ragu-manjegowda/vivitsa.svg?branch=master)](https://travis-ci.org/ragu-manjegowda/vivitsa)

# Vivitsa
  My operating system - from scratch!!!

<p align="center"> <a href="https://www.youtube.com/watch?v=Cbior8aP4nI" target="_blank"> Click here to see the demo on YouTube </a></p>

<p align="center"><a href="http://www.youtube.com/watch?feature=player_embedded&v=Cbior8aP4nI
" target="_blank"><img src="https://img.youtube.com/vi/Cbior8aP4nI/0.jpg" 
alt="Click to view Introduction Video" width="480" height="360" border="10" /></a></p>

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

And then follow README file inside subfolders on instructions to build and debug

Note: When you are inside docker, host PCs HOME directory is mounted under /mnt

### To run, debug and to know more please refer README.md in sub-folders.


# References & Tutorials

There are a lot of great resources out there but not many good tutorials. Most of the tutorials gloss over the code or gloss over the contents, or both; it forces you to spend time reading about theory and finding other implementations of concepts but it can be a pain when you just want X to work so you can learn about Y.

This is just a list of some of the great stuff I've found online, including others' toy kernels. For better or worse, many of them are based on the same few tutorials found elsewhere so, often, the code is easy enough to follow.

 - [Little OS Book](https://littleosbook.github.io/) - May not be maintained anymore, as of late 2015. There are some bugs referenced on the [GitHub repo](https://github.com/littleosbook/littleosbook). It starts off with a lot of example code but then tapers off by only providing theory and leaving you to your own devices. It was great to get me started but I needed a bit more help on interrupts and memory management.

 - [aenix](https://github.com/helino/aenix) from the authors of the Little OS Book tutorial.

 - [OSDev.org](http://wiki.osdev.org/Main_Page) - The best resource on the net, with dozens of tutorials and even several pages (linked below) providing information on bugs in other tutorials.

 - [James Molloy's Tutorial](http://web.archive.org/web/20120223002615/http://www.jamesmolloy.co.uk/tutorial_html/1.-Environment%20setup.html) - [No longer maintained](http://forum.osdev.org/viewtopic.php?f=1&t=23072&start=0) but a great intro nonetheless. Newer, working code is in the [Google Code repo](https://code.google.com/archive/p/jamesm-tutorials/), but it doesn't match the tutorial. Since Google Code was decommissioned, you can only download a ZIP or export to GitHub; there's no online browsing of the SVN repo.

 - [Boom](https://github.com/BiggerOnTheInside/Boom) - A toy kernel based on JM's tutorial.

 - [Bare Metal Examples](https://github.com/cirosantilli/x86-bare-metal-examples) - Multiple toy OSes for reference.

 - [Bran's Kernel Dev Tutorial](http://www.osdever.net/bkerndev/Docs/title.htm) - Great tutorial, also with [known bugs](http://wiki.osdev.org/Bran's_Known_Bugs).

 - [MIT's Xv6](https://pdos.csail.mit.edu/6.828/2011/xv6.html) - Unix-like OS used to teach OS development at MIT.

 - [XOmB-barebones](https://github.com/xomboverlord/xomb-bare-bones) - Barebones x64 kernel written in D.

 - [ToaruOS](https://github.com/klange/toaruos) - Much more complex than all the others above but it's a genuinely functional Unix-like OS with a fantastic desktop environment and even some application ports, including GCC and Bochs, which means it can be self-hosting!
