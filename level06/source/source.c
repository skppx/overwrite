#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>

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
