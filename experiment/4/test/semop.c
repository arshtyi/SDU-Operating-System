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
    struct sembuf p_op = {0, -1, 0};
    struct sembuf v_op = {0, +1, 0};
    union semun arg;

    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);
    if (semid < 0) {
        fprintf(stderr, "[FAIL] semget: %s\n", strerror(errno));
        return 1;
    }

    arg.val = 1;
    if (semctl(semid, 0, SETVAL, arg) < 0) {
        fprintf(stderr, "[FAIL] semctl(SETVAL): %s\n", strerror(errno));
        semctl(semid, 0, IPC_RMID);
        return 1;
    }

    if (semop(semid, &p_op, 1) < 0) {
        fprintf(stderr, "[FAIL] semop(P): %s\n", strerror(errno));
        semctl(semid, 0, IPC_RMID);
        return 1;
    }

    if (semop(semid, &v_op, 1) < 0) {
        fprintf(stderr, "[FAIL] semop(V): %s\n", strerror(errno));
        semctl(semid, 0, IPC_RMID);
        return 1;
    }

    printf("[PASS] semop P/V operations succeeded\n");
    printf("[INFO] keep alive for 30s so ipcs can see it, then auto IPC_RMID\n");
    sleep(30);

    if (semctl(semid, 0, IPC_RMID) < 0) {
        fprintf(stderr, "[FAIL] semctl(IPC_RMID): %s\n", strerror(errno));
        return 1;
    }

    return 0;
}
