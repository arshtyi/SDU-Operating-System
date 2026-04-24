#include "ipc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define BARBERS 3
#define CHAIRS 3
#define SOFA_CAP 4
#define WAIT_CAP 13
#define MAX_INSHOP 20
#define MAX_CUST 64

#define SEM_MUTEX 0
#define SEM_EVENT 1
#define SEM_CASH 2
#define SEM_CALL_BASE 3
#define SEM_DONE_BASE (SEM_CALL_BASE + MAX_CUST)
#define SEM_PAY_BASE (SEM_DONE_BASE + MAX_CUST)
#define SEM_TOTAL (SEM_PAY_BASE + MAX_CUST)

typedef struct {
    int a[WAIT_CAP];
    int h;
    int t;
    int n;
} WaitQ;

typedef struct {
    int a[SOFA_CAP];
    int h;
    int t;
    int n;
} SofaQ;

typedef struct {
    int a[MAX_CUST];
    int h;
    int t;
    int n;
} PayQ;

typedef struct {
    int open;
    int inside;
    int served;
    int rejected;
    WaitQ waitq;
    SofaQ sofaq;
    PayQ payq;
} Shop;

static int push_wait(WaitQ* q, int id) {
    if (q->n >= WAIT_CAP) return -1;
    q->a[q->t] = id;
    q->t = (q->t + 1) % WAIT_CAP;
    q->n++;
    return 0;
}

static int pop_wait(WaitQ* q) {
    int id;
    if (q->n <= 0) return -1;
    id = q->a[q->h];
    q->h = (q->h + 1) % WAIT_CAP;
    q->n--;
    return id;
}

static int push_sofa(SofaQ* q, int id) {
    if (q->n >= SOFA_CAP) return -1;
    q->a[q->t] = id;
    q->t = (q->t + 1) % SOFA_CAP;
    q->n++;
    return 0;
}

static int pop_sofa(SofaQ* q) {
    int id;
    if (q->n <= 0) return -1;
    id = q->a[q->h];
    q->h = (q->h + 1) % SOFA_CAP;
    q->n--;
    return id;
}

static int push_pay(PayQ* q, int id) {
    if (q->n >= MAX_CUST) return -1;
    q->a[q->t] = id;
    q->t = (q->t + 1) % MAX_CUST;
    q->n++;
    return 0;
}

static int pop_pay(PayQ* q) {
    int id;
    if (q->n <= 0) return -1;
    id = q->a[q->h];
    q->h = (q->h + 1) % MAX_CUST;
    q->n--;
    return id;
}

static unsigned short sem_call(int id) { return (unsigned short)(SEM_CALL_BASE + id - 1); }
static unsigned short sem_done(int id) { return (unsigned short)(SEM_DONE_BASE + id - 1); }
static unsigned short sem_pay(int id) { return (unsigned short)(SEM_PAY_BASE + id - 1); }

static void msleep(int ms) {
    if (ms > 0) usleep((useconds_t)ms * 1000U);
}

static void customer(int id, int semid, Shop* s, int pay_ms) {
    srand((unsigned int)(time(NULL) ^ getpid()));

    ipc_p(semid, SEM_MUTEX);
    if (s->inside >= MAX_INSHOP) {
        s->rejected++;
        printf("[C%02d] full -> leave\n", id);
        fflush(stdout);
        ipc_v(semid, SEM_MUTEX);
        _exit(0);
    }

    s->inside++;
    if (s->sofaq.n < SOFA_CAP && s->waitq.n == 0) {
        push_sofa(&s->sofaq, id);
        printf("[C%02d] enter -> sofa\n", id);
        fflush(stdout);
        ipc_v(semid, SEM_EVENT);
    } else {
        if (push_wait(&s->waitq, id) < 0) {
            s->inside--;
            s->rejected++;
            printf("[C%02d] no seat -> leave\n", id);
            fflush(stdout);
            ipc_v(semid, SEM_MUTEX);
            _exit(0);
        }
        printf("[C%02d] enter -> waiting room\n", id);
        fflush(stdout);
    }
    ipc_v(semid, SEM_MUTEX);

    ipc_p(semid, sem_call(id));
    printf("[C%02d] in chair\n", id);
    fflush(stdout);

    ipc_p(semid, sem_done(id));

    ipc_p(semid, SEM_MUTEX);
    push_pay(&s->payq, id);
    printf("[C%02d] wait payment\n", id);
    fflush(stdout);
    ipc_v(semid, SEM_EVENT);
    ipc_v(semid, SEM_MUTEX);

    ipc_p(semid, sem_pay(id));
    msleep(pay_ms / 2 + rand() % (pay_ms + 1));

    ipc_p(semid, SEM_MUTEX);
    s->inside--;
    s->served++;
    printf("[C%02d] done -> leave\n", id);
    fflush(stdout);
    ipc_v(semid, SEM_MUTEX);
    _exit(0);
}

static void barber(int id, int semid, Shop* s, int cut_ms, int pay_ms) {
    srand((unsigned int)(time(NULL) ^ getpid()));
    while (1) {
        int cid = -1;
        int payment = 0;

        printf("[B%d] sleep\n", id);
        fflush(stdout);
        ipc_p(semid, SEM_EVENT);

        ipc_p(semid, SEM_MUTEX);
        if (!s->open && s->waitq.n == 0 && s->sofaq.n == 0 && s->payq.n == 0) {
            ipc_v(semid, SEM_MUTEX);
            break;
        }

        if (s->payq.n > 0) {
            cid = pop_pay(&s->payq);
            payment = 1;
        } else if (s->sofaq.n > 0) {
            cid = pop_sofa(&s->sofaq);
            if (s->waitq.n > 0) {
                int move = pop_wait(&s->waitq);
                if (move > 0 && push_sofa(&s->sofaq, move) == 0) {
                    printf("[B%d] move C%02d waiting->sofa\n", id, move);
                    fflush(stdout);
                    ipc_v(semid, SEM_EVENT);
                }
            }
        }
        ipc_v(semid, SEM_MUTEX);

        if (cid <= 0) continue;

        if (payment) {
            ipc_p(semid, SEM_CASH);
            printf("[B%d] charge C%02d\n", id, cid);
            fflush(stdout);
            msleep(pay_ms / 2 + rand() % (pay_ms + 1));
            ipc_v(semid, SEM_CASH);
            ipc_v(semid, sem_pay(cid));
            continue;
        }

        printf("[B%d] cut C%02d\n", id, cid);
        fflush(stdout);
        ipc_v(semid, sem_call(cid));
        msleep(cut_ms / 2 + rand() % (cut_ms + 1));
        printf("[B%d] finish C%02d\n", id, cid);
        fflush(stdout);
        ipc_v(semid, sem_done(cid));
    }

    printf("[B%d] close\n", id);
    fflush(stdout);
    _exit(0);
}

int main(int argc, char* argv[]) {
    int n = 30;
    int arrival_ms = 300;
    int cut_ms = 1000;
    int pay_ms = 400;
    int shmid;
    int semid;
    Shop* s;
    pid_t bp[BARBERS];
    pid_t cp[MAX_CUST];
    int i;

    if (argc > 1) n = atoi(argv[1]);
    if (argc > 2) arrival_ms = atoi(argv[2]);
    if (argc > 3) cut_ms = atoi(argv[3]);
    if (argc > 4) pay_ms = atoi(argv[4]);
    if (n < 1) n = 1;
    if (n > MAX_CUST) n = MAX_CUST;

    shmid = shmget(IPC_PRIVATE, sizeof(Shop), IPC_CREAT | 0600);
    if (shmid < 0) {
        perror("shmget");
        return 1;
    }
    s = (Shop*)shmat(shmid, NULL, 0);
    if (s == (void*)-1) {
        perror("shmat");
        shmctl(shmid, IPC_RMID, NULL);
        return 1;
    }
    memset(s, 0, sizeof(*s));
    s->open = 1;

    semid = ipc_sem_create(IPC_PRIVATE, SEM_TOTAL, IPC_CREAT | 0600);
    ipc_sem_set(semid, SEM_MUTEX, 1);
    ipc_sem_set(semid, SEM_EVENT, 0);
    ipc_sem_set(semid, SEM_CASH, 1);
    for (i = 0; i < MAX_CUST; i++) {
        ipc_sem_set(semid, SEM_CALL_BASE + i, 0);
        ipc_sem_set(semid, SEM_DONE_BASE + i, 0);
        ipc_sem_set(semid, SEM_PAY_BASE + i, 0);
    }

    printf("shop: barbers=%d chairs=%d sofa=%d wait=%d max=%d customers=%d\n",
           BARBERS,
           CHAIRS,
           SOFA_CAP,
           WAIT_CAP,
           MAX_INSHOP,
           n);
    fflush(stdout);

    for (i = 0; i < BARBERS; i++) {
        pid_t p = fork();
        if (p == 0) barber(i + 1, semid, s, cut_ms, pay_ms);
        bp[i] = p;
    }

    for (i = 0; i < n; i++) {
        pid_t p = fork();
        if (p == 0) customer(i + 1, semid, s, pay_ms);
        cp[i] = p;
        msleep(arrival_ms);
    }

    for (i = 0; i < n; i++) {
        if (cp[i] > 0) waitpid(cp[i], NULL, 0);
    }

    ipc_p(semid, SEM_MUTEX);
    s->open = 0;
    ipc_v(semid, SEM_MUTEX);
    for (i = 0; i < BARBERS; i++) ipc_v(semid, SEM_EVENT);
    for (i = 0; i < BARBERS; i++) if (bp[i] > 0) waitpid(bp[i], NULL, 0);

    printf("summary: served=%d rejected=%d inside=%d\n", s->served, s->rejected, s->inside);
    fflush(stdout);

    shmdt(s);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);
    return 0;
}