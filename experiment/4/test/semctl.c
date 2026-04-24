#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int main(void)
{
    union semun arg;

    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);
    if (semid < 0) {
        fprintf(stderr, "[FAIL] semget: %s\n", strerror(errno));
        return 1;
    }

    arg.val = 3;
    if (semctl(semid, 0, SETVAL, arg) < 0) {
        fprintf(stderr, "[FAIL] semctl(SETVAL): %s\n", strerror(errno));
        semctl(semid, 0, IPC_RMID);
        return 1;
    }

    int cur = semctl(semid, 0, GETVAL);
    if (cur < 0) {
        fprintf(stderr, "[FAIL] semctl(GETVAL): %s\n", strerror(errno));
        semctl(semid, 0, IPC_RMID);
        return 1;
    }

    if (cur != 3) {
        fprintf(stderr, "[FAIL] semctl value mismatch: expected=3 got=%d\n", cur);
        semctl(semid, 0, IPC_RMID);
        return 1;
    }

    printf("[INFO] keep alive for 30s so ipcs can see it, then auto IPC_RMID\n");
    sleep(30);

    if (semctl(semid, 0, IPC_RMID) < 0) {
        fprintf(stderr, "[FAIL] semctl(IPC_RMID): %s\n", strerror(errno));
        return 1;
    }

    errno = 0;
    if (semctl(semid, 0, GETVAL) >= 0) {
        fprintf(stderr, "[FAIL] semaphore array still exists after IPC_RMID\n");
        return 1;
    }

    if (errno != EINVAL && errno != EIDRM) {
        fprintf(stderr, "[FAIL] unexpected errno after delete: %s\n", strerror(errno));
        return 1;
    }

    printf("[PASS] semctl SETVAL/GETVAL/RMID succeeded\n");
    return 0;
}
