#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int main(void)
{
    struct msqid_ds ds;

    int msqid = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (msqid < 0) {
        fprintf(stderr, "[FAIL] msgget: %s\n", strerror(errno));
        return 1;
    }

    if (msgctl(msqid, IPC_STAT, &ds) < 0) {
        fprintf(stderr, "[FAIL] msgctl(IPC_STAT): %s\n", strerror(errno));
        msgctl(msqid, IPC_RMID, NULL);
        return 1;
    }

    printf("[PASS] msgctl IPC_STAT ok: msqid=%d, qnum=%lu, qbytes=%lu\n",
           msqid,
           (unsigned long)ds.msg_qnum,
           (unsigned long)ds.msg_qbytes);
    printf("[INFO] keep alive for 30s so ipcs can see it, then auto IPC_RMID\n");
    sleep(30);
    if (msgctl(msqid, IPC_RMID, NULL) < 0) {
        fprintf(stderr, "[FAIL] msgctl(IPC_RMID): %s\n", strerror(errno));
        return 1;
    }

    return 0;
}
