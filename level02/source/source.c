#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
