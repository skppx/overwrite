# Solution

Here is the decompile C code:

```c
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
```

So it's pretty simple we see that you have a call to `scanf`, 
and then a if statement that check if the result is 5276.

So what do we need to do is pretty simple just pass 5276 as value.


```sh
level00@OverRide:~$ ./level00
***********************************
*            -Level00 -           *
***********************************
Password:5276

Authenticated!
$ whoami
level01
$ cat /home/users/level01/.pass
uSq2ehEGT6c9S24zbshexZQBXUGrncxn5sD5QfGL
```
