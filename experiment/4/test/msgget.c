#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
    int msqid = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (msqid < 0) {
        fprintf(stderr, "[FAIL] msgget: %s\n", strerror(errno));
        return 1;
    }

    printf("[PASS] msgget created queue: msqid=%d\n", msqid);
    printf("[INFO] keep alive for 30s so ipcs can see it, then auto IPC_RMID\n");
    sleep(30);
    if (msgctl(msqid, IPC_RMID, NULL) < 0) {
        fprintf(stderr, "[FAIL] msgctl(IPC_RMID): %s\n", strerror(errno));
        return 1;
    }

    return 0;
}
