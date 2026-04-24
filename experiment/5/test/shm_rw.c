#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

int main(void) {
    const char* msg = "hello shm";
    int shmid = shmget(IPC_PRIVATE, 128, IPC_CREAT | 0600);
    if (shmid < 0) {
        fprintf(stderr, "[FAIL] shmget: %s\n", strerror(errno));
        return 1;
    }

    char* addr = (char*)shmat(shmid, NULL, 0);
    if (addr == (char*)-1) {
        fprintf(stderr, "[FAIL] shmat: %s\n", strerror(errno));
        shmctl(shmid, IPC_RMID, NULL);
        return 1;
    }

    strcpy(addr, msg);
    if (strcmp(addr, msg) != 0) {
        fprintf(stderr, "[FAIL] shm read/write mismatch, got=\"%s\"\n", addr);
        shmdt(addr);
        shmctl(shmid, IPC_RMID, NULL);
        return 1;
    }

    printf("[PASS] shm read/write ok: \"%s\"\n", addr);

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
