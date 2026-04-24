#include "ipc.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>

union semun {
    int val;
    struct semid_ds* buf;
    unsigned short* array;
};

int ipc_sem_create(key_t key, int nsems, int flags) {
    int semid = semget(key, nsems, flags);
    if (semid < 0) {
        perror("semget");
        exit(EXIT_FAILURE);
    }
    return semid;
}

int ipc_sem_set(int semid, int semnum, int value) {
    union semun arg;
    arg.val = value;
    if (semctl(semid, semnum, SETVAL, arg) < 0) {
        perror("semctl SETVAL");
        return -1;
    }
    return 0;
}

int ipc_p(int semid, unsigned short semnum) {
    struct sembuf op;
    op.sem_num = semnum;
    op.sem_op = -1;
    op.sem_flg = 0;
    if (semop(semid, &op, 1) < 0) {
        perror("semop P");
        return -1;
    }
    return 0;
}

int ipc_v(int semid, unsigned short semnum) {
    struct sembuf op;
    op.sem_num = semnum;
    op.sem_op = 1;
    op.sem_flg = 0;
    if (semop(semid, &op, 1) < 0) {
        perror("semop V");
        return -1;
    }
    return 0;
}