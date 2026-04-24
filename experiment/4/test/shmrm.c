#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
    int shmid = shmget(IPC_PRIVATE, 128, IPC_CREAT | 0600);
    if (shmid < 0) {
        fprintf(stderr, "[FAIL] shmget: %s\n", strerror(errno));
        return 1;
    }

    printf("[INFO] keep alive for 30s so ipcs can see it, then auto IPC_RMID\n");
    sleep(30);

    if (shmctl(shmid, IPC_RMID, NULL) < 0) {
        fprintf(stderr, "[FAIL] shmctl(IPC_RMID): %s\n", strerror(errno));
        return 1;
    }

    errno = 0;
    if (shmctl(shmid, IPC_STAT, NULL) == 0) {
        fprintf(stderr, "[FAIL] segment still exists after IPC_RMID\n");
        return 1;
    }

    if (errno != EINVAL && errno != EIDRM) {
        fprintf(stderr, "[FAIL] unexpected errno after delete: %s\n", strerror(errno));
        return 1;
    }

    printf("[PASS] shm segment removed, follow-up shmctl failed as expected: %s\n", strerror(errno));
    return 0;
}
