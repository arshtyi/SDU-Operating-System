#include "pctl.h"
#include <errno.h>

static volatile sig_atomic_t g_wakeup = 0;
static volatile sig_atomic_t g_stop = 0;

static void wakeup_handler(int signo)
{
    (void)signo;
    g_wakeup = 1;
}

static void stop_handler(int signo)
{
    (void)signo;
    g_stop = 1;
}

int main(void)
{
    if (signal(SIGINT, stop_handler) == SIG_ERR)
    {
        perror("signal(SIGINT) failed");
        return EXIT_FAILURE;
    }

    while (!g_stop)
    {
        pid_t child1_pid;
        pid_t child2_pid;
        int status;

        printf("=== new round ===\n");

        child1_pid = fork();
        if (child1_pid < 0)
        {
            perror("fork child1 failed");
            sleep(3);
            continue;
        }
        else if (child1_pid == 0)
        {
            if (signal(SIGUSR1, wakeup_handler) == SIG_ERR)
            {
                perror("signal failed in child1");
                _exit(EXIT_FAILURE);
            }

            printf("[child1:%d] waiting for wakeup, then run ls\n", getpid());
            while (!g_wakeup && !g_stop)
            {
                pause();
            }

            if (g_stop)
            {
                _exit(EXIT_SUCCESS);
            }

            execlp("ls", "ls", (char *)NULL);
            perror("execlp ls failed");
            _exit(EXIT_FAILURE);
        }

        child2_pid = fork();
        if (child2_pid < 0)
        {
            perror("fork child2 failed");
            kill(child1_pid, SIGKILL);
            waitpid(child1_pid, NULL, 0);
            sleep(3);
            continue;
        }
        else if (child2_pid == 0)
        {
            printf("[child2:%d] run ps first\n", getpid());
            execlp("ps", "ps", (char *)NULL);
            perror("execlp ps failed");
            _exit(EXIT_FAILURE);
        }

        while (waitpid(child2_pid, &status, 0) < 0)
        {
            if (errno == EINTR && g_stop)
            {
                kill(child2_pid, SIGKILL);
                waitpid(child2_pid, NULL, 0);
                break;
            }
            if (errno != EINTR)
            {
                perror("waitpid child2 failed");
                break;
            }
        }
        if (g_stop)
        {
            kill(child1_pid, SIGKILL);
            waitpid(child1_pid, NULL, 0);
            break;
        }
        if (kill(child1_pid, SIGUSR1) < 0)
        {
            perror("kill(SIGUSR1) child1 failed");
        }
        while (waitpid(child1_pid, &status, 0) < 0)
        {
            if (errno == EINTR && g_stop)
            {
                kill(child1_pid, SIGKILL);
                waitpid(child1_pid, NULL, 0);
                break;
            }
            if (errno != EINTR)
            {
                perror("waitpid child1 failed");
                break;
            }
        }

        if (!g_stop)
        {
            sleep(3);
        }
    }

    return EXIT_SUCCESS;
}
