#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

struct msgbuf_t {
    long mtype;
    char mtext[64];
};

int main(void)
{
    struct msgbuf_t msg;

    int msqid = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (msqid < 0) {
        fprintf(stderr, "[FAIL] msgget: %s\n", strerror(errno));
        return 1;
    }

    msg.mtype = 1;
    strncpy(msg.mtext, "hello msg", sizeof(msg.mtext) - 1);
    msg.mtext[sizeof(msg.mtext) - 1] = '\0';

    if (msgsnd(msqid, &msg, strlen(msg.mtext) + 1, 0) < 0) {
        fprintf(stderr, "[FAIL] msgsnd: %s\n", strerror(errno));
        msgctl(msqid, IPC_RMID, NULL);
        return 1;
    }

    printf("[PASS] msgsnd sent one message to msqid=%d\n", msqid);
    printf("[INFO] keep alive for 30s so ipcs can see it, then auto IPC_RMID\n");
    sleep(30);
    if (msgctl(msqid, IPC_RMID, NULL) < 0) {
        fprintf(stderr, "[FAIL] msgctl(IPC_RMID): %s\n", strerror(errno));
        return 1;
    }

    return 0;
}
