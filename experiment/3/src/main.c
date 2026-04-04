#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#define MAX_LINE 1024
#define MAX_ARGS 128
#define MAX_CMDS 16
#define HISTORY_SIZE 30

typedef struct {
    char *argv[MAX_ARGS];
    int argc;
    char *infile;
    char *outfile;
} Command;

typedef struct {
    Command cmds[MAX_CMDS];
    int cmd_count;
    int background;
    char storage[MAX_LINE * 3 + 1];
} ParsedLine;

static volatile sig_atomic_t g_foreground_pgid = 0;

static char g_history[HISTORY_SIZE][MAX_LINE + 1];
static int g_hist_start = 0;
static int g_hist_count = 0;

static void trim_trailing_ws(char *s)
{
    size_t n = strlen(s);
    while (n > 0 && isspace((unsigned char)s[n - 1])) {
        s[n - 1] = '\0';
        --n;
    }
}

static char *skip_leading_ws(char *s)
{
    while (*s != '\0' && isspace((unsigned char)*s)) {
        ++s;
    }
    return s;
}

static void sigint_handler(int sig)
{
    (void)sig;
    if (g_foreground_pgid > 0) {
        kill(-g_foreground_pgid, SIGINT);
    }
    (void)write(STDOUT_FILENO, "\n", 1);
}

static void setup_signal_handlers(void)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGINT, &sa, NULL) < 0) {
        perror("sigaction(SIGINT)");
        exit(1);
    }

    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
}

static void history_add(const char *line)
{
    int idx;

    if (line == NULL || line[0] == '\0') {
        return;
    }

    if (g_hist_count > 0) {
        int last = (g_hist_start + g_hist_count - 1) % HISTORY_SIZE;
        if (strcmp(g_history[last], line) == 0) {
            return;
        }
    }

    if (g_hist_count < HISTORY_SIZE) {
        idx = (g_hist_start + g_hist_count) % HISTORY_SIZE;
        ++g_hist_count;
    } else {
        idx = g_hist_start;
        g_hist_start = (g_hist_start + 1) % HISTORY_SIZE;
    }

    strncpy(g_history[idx], line, MAX_LINE);
    g_history[idx][MAX_LINE] = '\0';
}

static const char *history_get(int order)
{
    int idx;
    if (order < 0 || order >= g_hist_count) {
        return NULL;
    }
    idx = (g_hist_start + order) % HISTORY_SIZE;
    return g_history[idx];
}

static void redraw_line(const char *prompt, const char *buf)
{
    printf("\r\033[2K%s%s", prompt, buf);
    fflush(stdout);
}

static int read_command_line(const char *prompt, char *out, size_t out_size)
{
    struct termios oldt;
    struct termios raw;
    int use_raw = isatty(STDIN_FILENO);
    size_t len = 0;
    int hist_cursor = g_hist_count;
    char stash[MAX_LINE + 1] = {0};
    int stash_valid = 0;

    if (out_size == 0) {
        return -1;
    }

    out[0] = '\0';

    if (!use_raw) {
        if (fgets(out, (int)out_size, stdin) == NULL) {
            return 0;
        }
        trim_trailing_ws(out);
        return 1;
    }

    if (tcgetattr(STDIN_FILENO, &oldt) < 0) {
        perror("tcgetattr");
        return -1;
    }

    raw = oldt;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) < 0) {
        perror("tcsetattr");
        return -1;
    }

    printf("%s", prompt);
    fflush(stdout);

    while (1) {
        char c;
        ssize_t n = read(STDIN_FILENO, &c, 1);
        if (n == 0) {
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            printf("\n");
            return 0;
        }
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            perror("read");
            return -1;
        }

        if (c == '\n' || c == '\r') {
            out[len] = '\0';
            printf("\n");
            break;
        }

        if (c == 3) {
            out[0] = '\0';
            printf("^C\n");
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            return 2;
        }

        if (c == 127 || c == '\b') {
            if (len > 0) {
                --len;
                out[len] = '\0';
                redraw_line(prompt, out);
            }
            continue;
        }

        if (c == 27) {
            char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) <= 0) {
                continue;
            }
            if (read(STDIN_FILENO, &seq[1], 1) <= 0) {
                continue;
            }
            if (seq[0] == '[' && (seq[1] == 'A' || seq[1] == 'B')) {
                const char *src = NULL;
                if (seq[1] == 'A') {
                    if (g_hist_count == 0) {
                        continue;
                    }
                    if (hist_cursor == g_hist_count && !stash_valid) {
                        strncpy(stash, out, MAX_LINE);
                        stash[MAX_LINE] = '\0';
                        stash_valid = 1;
                    }
                    if (hist_cursor > 0) {
                        --hist_cursor;
                    }
                    src = history_get(hist_cursor);
                } else {
                    if (hist_cursor < g_hist_count) {
                        ++hist_cursor;
                    }
                    if (hist_cursor == g_hist_count) {
                        src = stash_valid ? stash : "";
                    } else {
                        src = history_get(hist_cursor);
                    }
                }

                if (src != NULL) {
                    strncpy(out, src, out_size - 1);
                    out[out_size - 1] = '\0';
                    len = strlen(out);
                    redraw_line(prompt, out);
                }
            }
            continue;
        }

        if (isprint((unsigned char)c)) {
            if (len + 1 < out_size) {
                out[len++] = c;
                out[len] = '\0';
                redraw_line(prompt, out);
            }
            continue;
        }
    }

    if (tcsetattr(STDIN_FILENO, TCSANOW, &oldt) < 0) {
        perror("tcsetattr restore");
        return -1;
    }

    return 1;
}

static int normalize_line(const char *in, char *out, size_t out_size)
{
    size_t i;
    size_t j = 0;

    for (i = 0; in[i] != '\0'; ++i) {
        char c = in[i];
        int special = (c == '|' || c == '<' || c == '>' || c == '&');

        if (special) {
            if (j + 3 >= out_size) {
                return -1;
            }
            out[j++] = ' ';
            out[j++] = c;
            out[j++] = ' ';
        } else {
            if (j + 1 >= out_size) {
                return -1;
            }
            out[j++] = c;
        }
    }

    if (j >= out_size) {
        return -1;
    }
    out[j] = '\0';
    return 0;
}

static void init_parsed_line(ParsedLine *pl)
{
    int i;
    memset(pl, 0, sizeof(*pl));
    for (i = 0; i < MAX_CMDS; ++i) {
        pl->cmds[i].argc = 0;
        pl->cmds[i].infile = NULL;
        pl->cmds[i].outfile = NULL;
        pl->cmds[i].argv[0] = NULL;
    }
}

static int parse_line(char *line, ParsedLine *pl)
{
    char *token;
    int cmd_idx = 0;
    Command *cmd;

    init_parsed_line(pl);

    if (normalize_line(line, pl->storage, sizeof(pl->storage)) < 0) {
        fprintf(stderr, "error: command too long\n");
        return -1;
    }

    token = strtok(pl->storage, " \t");
    if (token == NULL) {
        return 0;
    }

    pl->cmd_count = 1;
    cmd = &pl->cmds[cmd_idx];

    while (token != NULL) {
        if (strcmp(token, "|") == 0) {
            if (cmd->argc == 0) {
                fprintf(stderr, "error: empty command near '|'\n");
                return -1;
            }
            if (pl->cmd_count >= MAX_CMDS) {
                fprintf(stderr, "error: too many pipeline commands\n");
                return -1;
            }
            cmd->argv[cmd->argc] = NULL;
            ++cmd_idx;
            ++pl->cmd_count;
            cmd = &pl->cmds[cmd_idx];
        } else if (strcmp(token, "<") == 0) {
            token = strtok(NULL, " \t");
            if (token == NULL || strcmp(token, "|") == 0 || strcmp(token, ">") == 0 || strcmp(token, "<") == 0 || strcmp(token, "&") == 0) {
                fprintf(stderr, "error: missing input file after '<'\n");
                return -1;
            }
            if (cmd->infile != NULL) {
                fprintf(stderr, "error: duplicate input redirection\n");
                return -1;
            }
            cmd->infile = token;
        } else if (strcmp(token, ">") == 0) {
            token = strtok(NULL, " \t");
            if (token == NULL || strcmp(token, "|") == 0 || strcmp(token, ">") == 0 || strcmp(token, "<") == 0 || strcmp(token, "&") == 0) {
                fprintf(stderr, "error: missing output file after '>'\n");
                return -1;
            }
            if (cmd->outfile != NULL) {
                fprintf(stderr, "error: duplicate output redirection\n");
                return -1;
            }
            cmd->outfile = token;
        } else if (strcmp(token, "&") == 0) {
            if (pl->background) {
                fprintf(stderr, "error: duplicate '&'\n");
                return -1;
            }
            pl->background = 1;
            token = strtok(NULL, " \t");
            if (token != NULL) {
                fprintf(stderr, "error: '&' must appear at end\n");
                return -1;
            }
            break;
        } else {
            if (cmd->argc >= MAX_ARGS - 1) {
                fprintf(stderr, "error: too many arguments\n");
                return -1;
            }
            cmd->argv[cmd->argc++] = token;
            cmd->argv[cmd->argc] = NULL;
        }

        token = strtok(NULL, " \t");
    }

    if (cmd->argc == 0) {
        fprintf(stderr, "error: empty command\n");
        return -1;
    }

    for (cmd_idx = 0; cmd_idx < pl->cmd_count; ++cmd_idx) {
        if (cmd_idx > 0 && pl->cmds[cmd_idx].infile != NULL) {
            fprintf(stderr, "error: '<' only supported in first command of pipeline\n");
            return -1;
        }
        if (cmd_idx < pl->cmd_count - 1 && pl->cmds[cmd_idx].outfile != NULL) {
            fprintf(stderr, "error: '>' only supported in last command of pipeline\n");
            return -1;
        }
    }

    return 1;
}

static int apply_redirection(const Command *cmd)
{
    int fd;

    if (cmd->infile != NULL) {
        fd = open(cmd->infile, O_RDONLY);
        if (fd < 0) {
            fprintf(stderr, "error: cannot open input '%s': %s\n", cmd->infile, strerror(errno));
            return -1;
        }
        if (dup2(fd, STDIN_FILENO) < 0) {
            perror("dup2(input)");
            close(fd);
            return -1;
        }
        close(fd);
    }

    if (cmd->outfile != NULL) {
        fd = open(cmd->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            fprintf(stderr, "error: cannot open output '%s': %s\n", cmd->outfile, strerror(errno));
            return -1;
        }
        if (dup2(fd, STDOUT_FILENO) < 0) {
            perror("dup2(output)");
            close(fd);
            return -1;
        }
        close(fd);
    }

    return 0;
}

static void reap_background(void)
{
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) {
            printf("[bg %d] exit %d\n", pid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("[bg %d] killed by signal %d\n", pid, WTERMSIG(status));
        }
    }
}

static int execute_parsed_line(const ParsedLine *pl)
{
    int i;
    int prev_read = -1;
    int pipefd[2] = {-1, -1};
    pid_t pids[MAX_CMDS];
    pid_t pgid = 0;

    for (i = 0; i < pl->cmd_count; ++i) {
        int has_next = (i < pl->cmd_count - 1);
        pid_t pid;

        if (has_next) {
            if (pipe(pipefd) < 0) {
                perror("pipe");
                if (prev_read != -1) {
                    close(prev_read);
                }
                return -1;
            }
        }

        pid = fork();
        if (pid < 0) {
            perror("fork");
            if (prev_read != -1) {
                close(prev_read);
            }
            if (has_next) {
                close(pipefd[0]);
                close(pipefd[1]);
            }
            return -1;
        }

        if (pid == 0) {
            signal(SIGINT, SIG_DFL);

            if (pgid == 0) {
                setpgid(0, 0);
            } else {
                setpgid(0, pgid);
            }

            if (prev_read != -1) {
                if (dup2(prev_read, STDIN_FILENO) < 0) {
                    perror("dup2(pipe in)");
                    _exit(1);
                }
            }

            if (has_next) {
                if (dup2(pipefd[1], STDOUT_FILENO) < 0) {
                    perror("dup2(pipe out)");
                    _exit(1);
                }
            }

            if (prev_read != -1) {
                close(prev_read);
            }
            if (has_next) {
                close(pipefd[0]);
                close(pipefd[1]);
            }

            if (apply_redirection(&pl->cmds[i]) < 0) {
                _exit(1);
            }

            execvp(pl->cmds[i].argv[0], pl->cmds[i].argv);
            fprintf(stderr, "error: command not found or cannot execute '%s': %s\n",
                pl->cmds[i].argv[0], strerror(errno));
            _exit(127);
        }

        if (pgid == 0) {
            pgid = pid;
        }
        setpgid(pid, pgid);
        pids[i] = pid;

        if (prev_read != -1) {
            close(prev_read);
        }
        if (has_next) {
            close(pipefd[1]);
            prev_read = pipefd[0];
        } else {
            prev_read = -1;
        }
    }

    if (pl->background) {
        printf("[bg] started pgid=%d\n", pgid);
        return 0;
    }

    g_foreground_pgid = pgid;
    for (i = 0; i < pl->cmd_count; ++i) {
        int status;
        while (waitpid(pids[i], &status, 0) < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("waitpid");
            break;
        }
    }
    g_foreground_pgid = 0;

    return 0;
}

int main(void)
{
    char line[MAX_LINE + 1];

    setup_signal_handlers();

    while (1) {
        ParsedLine parsed;
        char *cmd;
        int rc;

        reap_background();

        rc = read_command_line("# ", line, sizeof(line));
        if (rc == 0) {
            break;
        }
        if (rc < 0) {
            continue;
        }
        if (rc == 2) {
            continue;
        }

        trim_trailing_ws(line);
        cmd = skip_leading_ws(line);
        if (*cmd == '\0') {
            continue;
        }

        if (strcmp(cmd, "exit") == 0) {
            break;
        }

        history_add(cmd);

        rc = parse_line(cmd, &parsed);
        if (rc <= 0) {
            continue;
        }

        execute_parsed_line(&parsed);
    }

    return 0;
}
