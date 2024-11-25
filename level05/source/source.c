#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int ac, char **av, char **envp)
{
    char buffer[100]; 

    fgets(buffer, 100, stdin);
    for ( unsigned int i = 0; i < strlen(buffer); ++i )
    {
        if ( buffer[i] > 64 && buffer[i] <= 90 )
            buffer[i] ^= 0x20u; // buffer[i] = tolower(buffer[i]);
    }
    printf(buffer);
    exit(0);
}
