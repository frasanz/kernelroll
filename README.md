# Putting rickrolling in kernel space

Kernelroll is a linux kernel module for advanced rickrolling.

It works by patching the open() system call to open a specified music file
instead of other music files. Currently, it only checks if the file extension
"mp3" is present and calls the original open() with the supplied path
instead.

WARNING: There is probably a performance penalty and your kernel might crash
at a very inappropriate time and cause data loss! You are responsible for
the code you load into your kernel!

But most probably, it will be alright! ;)

# Installation

Tested in  3.2.0-4-amd64, and intel i7

Right now, the module finds by itself the address of the 
sys_call_table. 

Have a look at https://bbs.archlinux.org/viewtopic.php?id=139406 for details

Compile with:

    $ make

Load with:

    $ insmod kernelroll.ko rollfile=/path/to/rickroll.mp3

Fire up a music player of your choice, play a song and consider yourself
kernelrolled. ;)
