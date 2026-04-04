#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LINE 100
#define MAX_ARGS 32

static void trim_trailing_ws(char *s)
{
    size_t n = strlen(s);
    while (n > 0 && isspace((unsigned char)s[n - 1]))
    {
        s[n - 1] = '\0';
        --n;
    }
}

static char *skip_leading_ws(char *s)
{
    while (*s != '\0' && isspace((unsigned char)*s))
    {
        ++s;
    }
    return s;
}

static int parse_segment(char *segment, char *argv[], char **infile, char **outfile)
{
    int argc = 0;
    char *token = strtok(segment, " \t");

    *infile = NULL;
    *outfile = NULL;

    while (token != NULL)
    {
        if (strcmp(token, "<") == 0)
        {
            token = strtok(NULL, " \t");
            if (token == NULL)
            {
                fprintf(stderr, "missing input file after '<'\n");
                return -1;
            }
            *infile = token;
        }
        else if (strcmp(token, ">") == 0)
        {
            token = strtok(NULL, " \t");
            if (token == NULL)
            {
                fprintf(stderr, "missing output file after '>'\n");
                return -1;
            }
            *outfile = token;
        }
        else
        {
            if (argc >= MAX_ARGS - 1)
            {
                fprintf(stderr, "too many arguments\n");
                return -1;
            }
            argv[argc++] = token;
        }
        token = strtok(NULL, " \t");
    }

    argv[argc] = NULL;
    return argc;
}

static int apply_redirection(const char *infile, const char *outfile)
{
    int fd;

    if (infile != NULL)
    {
        fd = open(infile, O_RDONLY);
        if (fd < 0)
        {
            perror("open input");
            return -1;
        }
        if (dup2(fd, STDIN_FILENO) < 0)
        {
            perror("dup2 input");
            close(fd);
            return -1;
        }
        close(fd);
    }

    if (outfile != NULL)
    {
        fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0)
        {
            perror("open output");
            return -1;
        }
        if (dup2(fd, STDOUT_FILENO) < 0)
        {
            perror("dup2 output");
            close(fd);
            return -1;
        }
        close(fd);
    }

    return 0;
}

int main(void)
{
    char line[MAX_LINE + 2];

    signal(SIGINT, SIG_IGN);

    while (1)
    {
        int background = 0;
        char *pipe_pos;
        pid_t pid;

        printf("COMMAND->");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL)
        {
            putchar('\n');
            break;
        }

        trim_trailing_ws(line);
        {
            char *cmd = skip_leading_ws(line);
            if (*cmd == '\0')
            {
                continue;
            }
            if (strcmp(cmd, "exit") == 0)
            {
                break;
            }

            trim_trailing_ws(cmd);
            {
                size_t len = strlen(cmd);
                if (len > 0 && cmd[len - 1] == '&')
                {
                    background = 1;
                    cmd[len - 1] = '\0';
                    trim_trailing_ws(cmd);
                }
            }

            pipe_pos = strchr(cmd, '|');

            pid = fork();
            if (pid < 0)
            {
                perror("fork");
                continue;
            }

            if (pid == 0)
            {
                signal(SIGINT, SIG_DFL);

                if (pipe_pos != NULL)
                {
                    int fildes[2];
                    pid_t left_pid;
                    char *left = cmd;
                    char *right;
                    char *left_argv[MAX_ARGS];
                    char *right_argv[MAX_ARGS];
                    char *left_in = NULL;
                    char *left_out = NULL;
                    char *right_in = NULL;
                    char *right_out = NULL;

                    *pipe_pos = '\0';
                    right = pipe_pos + 1;
                    left = skip_leading_ws(left);
                    right = skip_leading_ws(right);
                    trim_trailing_ws(left);
                    trim_trailing_ws(right);

                    if (parse_segment(left, left_argv, &left_in, &left_out) <= 0)
                    {
                        _exit(1);
                    }
                    if (parse_segment(right, right_argv, &right_in, &right_out) <= 0)
                    {
                        _exit(1);
                    }

                    if (pipe(fildes) < 0)
                    {
                        perror("pipe");
                        _exit(1);
                    }

                    left_pid = fork();
                    if (left_pid < 0)
                    {
                        perror("fork for pipe left");
                        _exit(1);
                    }

                    if (left_pid == 0)
                    {
                        if (left_in != NULL)
                        {
                            if (apply_redirection(left_in, NULL) < 0)
                            {
                                _exit(1);
                            }
                        }
                        close(fildes[0]);
                        if (dup2(fildes[1], STDOUT_FILENO) < 0)
                        {
                            perror("dup2 pipe left");
                            _exit(1);
                        }
                        close(fildes[1]);
                        execvp(left_argv[0], left_argv);
                        perror("execvp left");
                        _exit(127);
                    }

                    close(fildes[1]);
                    if (dup2(fildes[0], STDIN_FILENO) < 0)
                    {
                        perror("dup2 pipe right");
                        _exit(1);
                    }
                    close(fildes[0]);

                    if (apply_redirection(right_in, right_out) < 0)
                    {
                        _exit(1);
                    }

                    execvp(right_argv[0], right_argv);
                    perror("execvp right");
                    _exit(127);
                }
                else
                {
                    char *argv[MAX_ARGS];
                    char *infile = NULL;
                    char *outfile = NULL;

                    if (parse_segment(cmd, argv, &infile, &outfile) <= 0)
                    {
                        _exit(1);
                    }
                    if (apply_redirection(infile, outfile) < 0)
                    {
                        _exit(1);
                    }

                    execvp(argv[0], argv);
                    perror("execvp");
                    _exit(127);
                }
            }

            if (background == 0)
            {
                int status;
                while (waitpid(pid, &status, 0) < 0)
                {
                    if (errno != EINTR)
                    {
                        perror("waitpid");
                        break;
                    }
                }
            }
        }
    }

    return 0;
}
