#include <unistd.h>
#include <memory.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <sys/ptrace.h>

int main(int argc, const char **argv, const char **envp)
{
  int   status;
  char  buffer[128];
  long  ret;
  pid_t pid;

  pid = fork();
  memset(buffer, 0, sizeof(buffer));
  ret = 0;
  status = 0;
  if ( pid )
  {
      // loop to check if child is dead or not also check everytime the EAX register,
      // if value is equal to 11 this would mean that execve() is called,
      // this will kill the child process and exit the main process if 11 is found.
    do
    {
      wait(&status);
      if (  WTERMSIG(status) || WIFSIGNALED(status) )
      {
        puts("child is exiting...");
        return 0;
      }
      ret = ptrace(PTRACE_PEEKUSER, pid, 44, 0);
    } while ( ret != 11 ); 
    puts("no exec() for you");
    kill(pid, 9);
  }
  else
  {
    prctl(1, 1); //
    ptrace(PTRACE_TRACEME, 0, 0, 0);
    puts("Give me some shellcode, k");
    gets(buffer);
  }
  return 0;
}
