#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(void)
{
    const char *path = "test_sys_open_rw.txt";
    const char *payload = "open/write/read test\n";
    char buf[128];

    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
    {
        perror("open for write");
        return 1;
    }
    if (write(fd, payload, strlen(payload)) < 0)
    {
        perror("write");
        close(fd);
        return 1;
    }
    if (close(fd) < 0)
    {
        perror("close write fd");
        return 1;
    }

    fd = open(path, O_RDONLY);
    if (fd < 0)
    {
        perror("open for read");
        return 1;
    }

    ssize_t n = read(fd, buf, sizeof(buf) - 1);
    if (n < 0)
    {
        perror("read");
        close(fd);
        return 1;
    }
    buf[n] = '\0';

    if (close(fd) < 0)
    {
        perror("close read fd");
        return 1;
    }

    printf("[file] %s", buf);
    return 0;
}
