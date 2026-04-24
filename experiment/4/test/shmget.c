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

    printf("[PASS] shmget created segment: shmid=%d\n", shmid);
    printf("[INFO] keep alive for 30s so ipcs can see it, then auto IPC_RMID\n");
    sleep(30);
    if (shmctl(shmid, IPC_RMID, NULL) < 0) {
        fprintf(stderr, "[FAIL] shmctl(IPC_RMID): %s\n", strerror(errno));
        return 1;
    }

    return 0;
}
