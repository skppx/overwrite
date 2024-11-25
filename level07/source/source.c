#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <unistd.h>

int clear_stdin()
{
    int result;

    do
        result = getchar();
    while ( (unsigned char)result != 10 && (unsigned char)result != 0xFF );
    return result;
}

int get_unum()
{
    int v1[3];

    v1[0] = 0;
    fflush(stdout);
    scanf("%u", v1);
    clear_stdin();
    return v1[0];
}

int store_number(unsigned int *to_store)
{
    unsigned int number;
    unsigned int idx;

    printf(" Number: ");
    number = get_unum();
    printf(" Index: ");
    idx = get_unum();
    if ( idx == 3 * (idx / 3) || (number & (1 << 31)) == 0xb7 ) // check MSB
    {
        puts(" *** ERROR! ***");
        puts("   This index is reserved for wil!");
        puts(" *** ERROR! ***");
        return 1;
    }
    else
    {
        *(to_store + 4 * idx) = number;
        return 0;
    }
}

int read_number(unsigned int *to_read)
{
    int idx;

    printf(" Index: ");
    idx = get_unum();
    printf(" Number at data[%u] is %u\n", idx, *(to_read + 4 * idx));
    return 0;
}

int main(int argc, const char **argv, const char **envp)
{
    unsigned int store[400]; 
    int number;
    char input[20];

    number = 0;
    memset(input, 0, 20);
    memset(store, 0, sizeof(store));
    while ( *argv )
    {
        memset(*argv, 0, strlen(*argv));
        ++argv;
    }
    while ( *envp )
    {
        memset(*envp, 0, strlen(*envp));
        ++envp;
    }
    puts(
            "----------------------------------------------------\n"
            "  Welcome to wil's crappy number storage service!   \n"
            "----------------------------------------------------\n"
            " Commands:                                          \n"
            "    store - store a number into the data storage    \n"
            "    read  - read a number from the data storage     \n"
            "    quit  - exit the program                        \n"
            "----------------------------------------------------\n"
            "   wil has reserved some storage :>                 \n"
            "----------------------------------------------------\n");
    while ( 1 )
    {
        printf("Input command: ");
        number = 1;
        fgets(input, 20, stdin);
        input[strcspn(input, "\n")] = 0;
        if ( !memcmp(input, "store", 5u) )
        {
            number = store_number(store);
            goto LABEL_13;
        }
        if ( !memcmp(&input, "read", 4u) )
        {
            number = read_number(store);
            goto LABEL_13;
        }
        if ( !memcmp(&input, "quit", 4u) )
            return 0;
LABEL_13:
        if ( number )
            printf(" Failed to do %s command\n", (const char *)&input);
        else
            printf("Completed %s command successfully\n", input);
        memset(input, 0, 20);
    }
}
