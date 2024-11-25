#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int decrypt(int a1)
{
    unsigned int i;
    unsigned int len;
    char    decrypt[29];

    strcpy(decrypt, "Q}|u`sfg~sf{}|a3");
    len = strlen(decrypt);
    for (i = 0; i < len; i++)
        decrypt[i] ^= a1;
    if (!strcmp(decrypt, "Congratulations!"))
        return (system("/bin/sh"));
    else
        return puts("\nInvalid Password");
}

int test(int a1, int a2)
{
    int result;
    int ret;

    switch (a2 - a1)
    {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
            result = decrypt(a2 - a1);
            break;
        default:
            ret = rand();
            result = decrypt(ret);
            break;
    }
    return result;
}

int main(void)
{
    int ret;
    int passwd;

    ret = time(0);
    srand(ret);
    puts("***********************************");
    puts("*\t\tlevel03\t\t**");
    puts("***********************************");
    printf("Password:");
    scanf("%d", &passwd);
    test(passwd, 322424845);
    return 0;
}
