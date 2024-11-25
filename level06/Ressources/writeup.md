# Solution

Here is the c decompile code:

```c
int auth(char *login, unsigned int serial)
{
    unsigned int    len;
    unsigned int    hash;

    login[strcspn(login, "\n")] = 0;
    len = strnlen(login, 32);
    if (len <= 5)
        return 1;
    if (ptrace(0,0,1,0) == -1)
    {
        puts("\x1B[32m.---------------------------.");
        puts("\x1B[31m| !! TAMPERING DETECTED !!  |");
        puts("\x1B[32m'---------------------------'");
        return 1;
    }
    else
    {
        hash = login[3] ^ 0x1337 + 0x5EEDED;
        for (unsigned int i = 0; i < len; i++)
        {
            if (login[i] <= 31)
                return 1;
            hash += (hash ^ login[i]) % 0x539;
        }
        return serial != hash;
    }
}

int main(int ac, char **av, char **envp)
{
    unsigned int    serial;
    char    login[32];

    puts("***********************************");
    puts("*\t\tlevel06\t\t  *");
    puts("***********************************");
    printf("-> Enter Login: ");
    fgets(login, 32, stdin);
    puts("***********************************");
    puts("***** NEW ACCOUNT DETECTED ********");
    puts("***********************************");
    printf("-> Enter Serial: ");
    scanf("%u", &serial);
    if (auth(login, serial))
        return 1;
    puts("Authenticated!");
    system("/bin/sh");
}
```

Here the goal is simple it's to pass the `auth()` function to do that we need to now what is the serial number,
that will be created by using our username that we provide so here is a simple python that will give us the serial based on our username,
it's pretty much a copy of the else block in the `auth()` function.

```python
#!/usr/bin/env python

i = 0
login = "qbornet"


start = ord(login[3]) ^ 0x1337 + 0x5EEDED;
print(f"value to start with {start}")
while i < len(login):
    start += (start ^ ord(login[i])) % 0x539
    i += 1
print(f"end value {start}")
# Output: value to start with 6226262
#         end value 6233769
```

```sh
level06@OverRide:~$ ./level06
***********************************
*               level06           *
***********************************
-> Enter Login: qbornet
***********************************
***** NEW ACCOUNT DETECTED ********
***********************************
-> Enter Serial: 6233769
Authenticated!
$ whoami
level07
$ cat /home/users/level07/.pass
GbcPDRgsFK77LNnnuh7QyFYA2942Gp8yKj9KrWD8
```

We got a shell and the flag to the next one !
