#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
    int pfd[2];
    if (pipe(pfd) < 0)
    {
        perror("pipe");
        return 1;
    }

    pid_t writer = fork();
    if (writer < 0)
    {
        perror("fork(writer)");
        return 1;
    }

    if (writer == 0)
    {
        const char *msg = "hello via pipe\n";
        close(pfd[0]);
        if (dup2(pfd[1], STDOUT_FILENO) < 0)
        {
            perror("dup2 writer");
            _exit(1);
        }
        close(pfd[1]);
        if (write(STDOUT_FILENO, msg, strlen(msg)) < 0)
        {
            perror("write");
            _exit(1);
        }
        _exit(0);
    }

    pid_t reader = fork();
    if (reader < 0)
    {
        perror("fork(reader)");
        return 1;
    }

    if (reader == 0)
    {
        char buf[64];
        ssize_t n;
        close(pfd[1]);
        if (dup2(pfd[0], STDIN_FILENO) < 0)
        {
            perror("dup2 reader");
            _exit(1);
        }
        close(pfd[0]);

        n = read(STDIN_FILENO, buf, sizeof(buf) - 1);
        if (n < 0)
        {
            perror("read");
            _exit(1);
        }
        buf[n] = '\0';
        printf("[reader] got: %s", buf);
        _exit(0);
    }

    close(pfd[0]);
    close(pfd[1]);

    int status = 0;
    while (wait(&status) > 0)
    {
    }
    if (errno != ECHILD)
    {
        perror("wait");
        return 1;
    }
    return 0;
}
