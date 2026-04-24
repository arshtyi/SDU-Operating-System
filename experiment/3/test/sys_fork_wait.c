#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        printf("[child] pid=%d ppid=%d\n", (int)getpid(), (int)getppid());
        _exit(42);
    }

    int status = 0;
    pid_t w = waitpid(pid, &status, 0);
    if (w < 0)
    {
        perror("waitpid");
        return 1;
    }

    if (WIFEXITED(status))
    {
        printf("[parent] child %d exited with code %d\n", (int)pid, WEXITSTATUS(status));
        return 0;
    }

    printf("[parent] child %d did not exit normally\n", (int)pid);
    return 1;
}
