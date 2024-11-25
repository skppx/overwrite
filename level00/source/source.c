#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int passwd;

    puts("***********************************");
    puts("* \t     -Level00 -\t\t  *");
    puts("***********************************");
    printf("Password:");
    scanf("%d", &passwd);
    if (passwd == 5276)
    {
        puts("\nAuthenticated!");
        system("/bin/sh");
        return 0;
    }
    else
    {
        puts("\nInvalid Password!");
        return 1;
    }

}
