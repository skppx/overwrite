# Solution

This is the C decompile binary:

```c
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
```


So from what we see we cannot find any exploit, but the decrypt function is important, since the decrypt function launch a shell,
we need to pass the if statement to get the shell.

To do so wee need to find the xor value applied on the decrypt string so let's create a simple python script that will match every known case 1-21.

```python
to_decrypt = "Q}|u`sfg~sf{}|a3"

for i in range(1, 21):
    decrypt = ""
    for j in to_decrypt:
        decrypt += chr(ord(j) ^ i)
    print(f"[{i}]: {decrypt}")

# [1]: P|}targfrgz|}`2
# [2]: S~wbqde|qdy~c1
# [3]: R~vcped}pex~b0
# [4]: Uyxqdwbczwbyxe7
# [5]: Txypevcb{vc~xyd6
# [6]: W{zsfu`axu`}{zg5
# [7]: Vz{rgta`yta|z{f4
# [8]: Yut}h{nov{nsuti;
# [9]: Xtu|izonwzortuh:
# [10]: [wvjylmtylqwvk9
# [11]: Zvw~kxmluxmpvwj8
# [12]: ]qpyljkrjwqpm?
# [13]: \pqxm~kjs~kvpql>
# [14]: _sr{n}hip}husro=
# [15]: ^rszo|ihq|itrsn<
# [16]: Amlepcvwncvkmlq#
# [17]: @lmdqbwvobwjlmp"
# [18]: Congratulations!
# [19]: Bnofs`utm`uhnor
# [20]: Eihatgrsjgroihu'
```

So the xor on 18 give us Congratulations! which is what we need, so now let's input a value that will allow us,
to obtain 18: `322424827`

Let's try it:

```sh
level03@OverRide:~$ ./level03 
***********************************
*               level03         **
***********************************
Password:322424827
$ whoami
level04
$ cat /home/users/level04/.pass
kgv3tkEb9h2mLkRsPkXRfc2mHbjMxQzvb2FrgKkf
```
