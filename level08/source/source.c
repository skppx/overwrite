#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

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
