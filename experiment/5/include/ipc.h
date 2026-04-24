#ifndef IPC_H
#define IPC_H

#include <sys/ipc.h>
#include <sys/types.h>

int ipc_sem_create(key_t key, int nsems, int flags);
int ipc_sem_set(int semid, int semnum, int value);
int ipc_p(int semid, unsigned short semnum);
int ipc_v(int semid, unsigned short semnum);

#endif
