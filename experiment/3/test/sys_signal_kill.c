#include <signal.h>
#include <stdio.h>
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
        for (;;)
        {
            pause();
        }
    }

    sleep(1);
    if (kill(pid, SIGINT) < 0)
    {
        perror("kill(SIGINT)");
        return 1;
    }

    int status = 0;
    if (waitpid(pid, &status, 0) < 0)
    {
        perror("waitpid");
        return 1;
    }

    if (WIFSIGNALED(status))
    {
        printf("[parent] child %d killed by signal %d\n", (int)pid, WTERMSIG(status));
        return 0;
    }

    printf("[parent] child %d did not terminate by signal\n", (int)pid);
    return 1;
}
