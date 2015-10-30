
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <errno.h>
#include <sys/wait.h>

#define BSIZE 256
#define READ_END 0
#define WRITE_END 1

#define BASH_EXEC  "/bin/bash"
#define FIND_EXEC  "/bin/find"
#define XARGS_EXEC "/usr/bin/xargs"
#define GREP_EXEC  "/bin/grep"
#define SORT_EXEC  "/bin/sort"
#define HEAD_EXEC  "/usr/bin/head"

int main(int argc, char *argv[])
{
  int status;
  pid_t pid_1, pid_2, pid_3, pid_4;
  int fd1[2], fd2[2], fd3[2];

  pipe(fd1);
  pipe(fd2);
  pipe(fd3);
  if (argc != 4) {
    printf("usage: finder DIR STR NUM_FILES\n");
    exit(0);
  }

  pid_1 = fork();
  if (pid_1 == 0) {
    /* First Child */
    dup2(fd1[WRITE_END],STDOUT_FILENO);
    char cmdbuf[BSIZE];
    bzero(cmdbuf, BSIZE);
    sprintf(cmdbuf, "%s %s -name \'*\'.[ch]", FIND_EXEC, argv[1]);
    /* set up pipes */
    close(fd1[READ_END]);
    close(fd1[WRITE_END]);
    close(fd2[READ_END]);
    close(fd2[WRITE_END]);
    close(fd3[READ_END]);
    close(fd3[WRITE_END]);

    if ( (execl(BASH_EXEC, BASH_EXEC, "-c", cmdbuf, (char *) 0)) < 0) {
      fprintf(stderr, "\nError execing find. ERROR#%d\n", errno);
      return EXIT_FAILURE;
    }

    exit(0);
  }

  pid_2 = fork();
  if (pid_2 == 0) {
    /* Second Child */
    char cmdbuf[BSIZE];
    bzero(cmdbuf, BSIZE);
    dup2(fd1[READ_END],STDIN_FILENO);
    dup2(fd2[WRITE_END],STDOUT_FILENO);

    sprintf(cmdbuf, "%s %s -c %s", XARGS_EXEC, GREP_EXEC, argv[2]);
   
    close(fd1[READ_END]);
    close(fd1[WRITE_END]);
    close(fd2[READ_END]);
    close(fd2[WRITE_END]);
    close(fd3[READ_END]);
    close(fd3[WRITE_END]);

    if ( (execl(BASH_EXEC, BASH_EXEC, "-c", cmdbuf, (char *) 0)) < 0) {
      fprintf(stderr, "\nError execing find. ERROR#%d\n", errno);
      return EXIT_FAILURE;
    }

    exit(0);
  }

  pid_3 = fork();
  if (pid_3 == 0) {
 
    char cmdbuf[BSIZE];
    bzero(cmdbuf, BSIZE);
    dup2(fd2[READ_END],STDIN_FILENO);
    dup2(fd3[WRITE_END],STDOUT_FILENO);

    sprintf(cmdbuf, "%s -t : +1.0 -2.0 --numeric --reverse", SORT_EXEC);

    close(fd1[READ_END]);
    close(fd1[WRITE_END]);
    close(fd2[READ_END]);
    close(fd2[WRITE_END]);
    close(fd3[READ_END]);
    close(fd3[WRITE_END]);


    if ( (execl(BASH_EXEC, BASH_EXEC, "-c", cmdbuf, (char *) 0)) < 0) {
      fprintf(stderr, "\nError execing find. ERROR#%d\n", errno);
      return EXIT_FAILURE;
    }

    exit(0);
  }
  
  pid_4 = fork();
  if (pid_4 == 0) {

    char cmdbuf[BSIZE];
    bzero(cmdbuf, BSIZE);
    dup2(fd3[READ_END],STDIN_FILENO);

    sprintf(cmdbuf, "%s --lines=%s",HEAD_EXEC, argv[3]);
    close(fd1[READ_END]);
    close(fd1[WRITE_END]);
    close(fd2[READ_END]);
    close(fd2[WRITE_END]);
    close(fd3[READ_END]);
    close(fd3[WRITE_END]);

    if ( (execl(BASH_EXEC, BASH_EXEC, "-c", cmdbuf, (char *) 0)) < 0) {
      fprintf(stderr, "\nError execing find. ERROR#%d\n", errno);
      return EXIT_FAILURE;
    }

    exit(0);
  }

    close(fd1[READ_END]);
    close(fd1[WRITE_END]);
    close(fd2[READ_END]);
    close(fd2[WRITE_END]);
    close(fd3[READ_END]);
    close(fd3[WRITE_END]);
  
  if ((waitpid(pid_1, &status, 0)) == -1) {
    fprintf(stderr, "Process 1 encountered an error. ERROR%d", errno);
    return EXIT_FAILURE;
  }
  if ((waitpid(pid_2, &status, 0)) == -1) {
    fprintf(stderr, "Process 2 encountered an error. ERROR%d", errno);
    return EXIT_FAILURE;
  }
  if ((waitpid(pid_3, &status, 0)) == -1) {
    fprintf(stderr, "Process 3 encountered an error. ERROR%d", errno);
    return EXIT_FAILURE;
  }
  if ((waitpid(pid_4, &status, 0)) == -1) {
    fprintf(stderr, "Process 4 encountered an error. ERROR%d", errno);
    return EXIT_FAILURE;
  }

  return 0;
}
