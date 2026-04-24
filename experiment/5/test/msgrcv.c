#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

struct msgbuf_t {
    long mtype;
    char mtext[64];
};

int main(void) {
    struct msgbuf_t send_msg;
    struct msgbuf_t recv_msg;

    int msqid = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
    if (msqid < 0) {
        fprintf(stderr, "[FAIL] msgget: %s\n", strerror(errno));
        return 1;
    }

    send_msg.mtype = 1;
    strncpy(send_msg.mtext, "read me", sizeof(send_msg.mtext) - 1);
    send_msg.mtext[sizeof(send_msg.mtext) - 1] = '\0';

    if (msgsnd(msqid, &send_msg, strlen(send_msg.mtext) + 1, 0) < 0) {
        fprintf(stderr, "[FAIL] msgsnd: %s\n", strerror(errno));
        msgctl(msqid, IPC_RMID, NULL);
        return 1;
    }

    if (msgrcv(msqid, &recv_msg, sizeof(recv_msg.mtext), 1, 0) < 0) {
        fprintf(stderr, "[FAIL] msgrcv: %s\n", strerror(errno));
        msgctl(msqid, IPC_RMID, NULL);
        return 1;
    }

    if (strcmp(recv_msg.mtext, "read me") != 0) {
        fprintf(stderr, "[FAIL] msgrcv data mismatch: got=\"%s\"\n", recv_msg.mtext);
        msgctl(msqid, IPC_RMID, NULL);
        return 1;
    }

    printf("[PASS] msgrcv received one message from msqid=%d: \"%s\"\n", msqid, recv_msg.mtext);
    printf("[INFO] keep alive for 30s so ipcs can see it, then auto IPC_RMID\n");
    sleep(30);
    if (msgctl(msqid, IPC_RMID, NULL) < 0) {
        fprintf(stderr, "[FAIL] msgctl(IPC_RMID): %s\n", strerror(errno));
        return 1;
    }

    return 0;
}
