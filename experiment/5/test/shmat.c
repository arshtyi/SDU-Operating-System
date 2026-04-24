#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

int main(void) {
    int shmid = shmget(IPC_PRIVATE, 128, IPC_CREAT | 0600);
    if (shmid < 0) {
        fprintf(stderr, "[FAIL] shmget: %s\n", strerror(errno));
        return 1;
    }

    void* addr = shmat(shmid, NULL, 0);
    if (addr == (void*)-1) {
        fprintf(stderr, "[FAIL] shmat: %s\n", strerror(errno));
        shmctl(shmid, IPC_RMID, NULL);
        return 1;
    }

    printf("[PASS] shmat attached at %p\n", addr);

    if (shmdt(addr) < 0) {
        fprintf(stderr, "[FAIL] shmdt: %s\n", strerror(errno));
        shmctl(shmid, IPC_RMID, NULL);
        return 1;
    }

    printf("[INFO] keep alive for 30s so ipcs can see it, then auto IPC_RMID\n");
    sleep(30);

    if (shmctl(shmid, IPC_RMID, NULL) < 0) {
        fprintf(stderr, "[FAIL] shmctl(IPC_RMID): %s\n", strerror(errno));
        return 1;
    }

    return 0;
}
