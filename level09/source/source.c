#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

int secret_backdoor(void)
{
    char s[128];

    fgets(s, 128, stdin);
    return system(s);
}

char    *set_msg(char *buffer)
{
    char s[1024];

    memset(s, 0, sizeof(s));
    puts(">: Msg @Unix-Dude");
    printf(">>: ");
    fgets(s, 1024, stdin);
    return strncpy(buffer, s, *(int *)(buffer + 180));
}

int    set_username(char *buffer)
{
    char s[140];
    int i;

    memset(s, 0, 0x80uLL);
    puts(">: Enter your username");
    printf(">>: ");
    fgets(s, 128, stdin);
    for ( i = 0; i <= 40 && s[i]; ++i )
        *(unsigned char *)(buffer + i + 140) = s[i];
    return printf(">: Welcome, %s", (const char *)(buffer + 140));
}

int    handle_msg(void)
{
    char buffer[140];

    set_username(buffer);
    set_msg(buffer);
    return puts(">: Msg sent!");
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
