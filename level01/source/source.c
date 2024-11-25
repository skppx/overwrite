#include <stdio.h>
#include <string.h>

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
