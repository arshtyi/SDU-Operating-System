#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
    int semid = semget(IPC_PRIVATE, 2, IPC_CREAT | 0600);
    if (semid < 0) {
        fprintf(stderr, "[FAIL] semget: %s\n", strerror(errno));
        return 1;
    }

    printf("[PASS] semget created semaphore array: semid=%d, nsems=2\n", semid);
    printf("[INFO] keep alive for 30s so ipcs can see it, then auto IPC_RMID\n");
    sleep(30);
    if (semctl(semid, 0, IPC_RMID) < 0) {
        fprintf(stderr, "[FAIL] semctl(IPC_RMID): %s\n", strerror(errno));
        return 1;
    }

    return 0;
}
