#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MANY_SEM_KEY 0x55A2
#define MANY_SUPPLIER_COUNT 5
#define MANY_SMOKER_COUNT 6

// sem layout:
// [0 .. MANY_SUPPLIER_COUNT-1] supplier turn semaphores
// [MANY_SUPPLIER_COUNT] table_empty
// [MANY_SUPPLIER_COUNT+1] smoker_tobacco
// [MANY_SUPPLIER_COUNT+2] smoker_paper
// [MANY_SUPPLIER_COUNT+3] smoker_glue
#define MANY_SEM_TABLE_EMPTY (MANY_SUPPLIER_COUNT)
#define MANY_SEM_SMOKER_TOBACCO (MANY_SUPPLIER_COUNT + 1)
#define MANY_SEM_SMOKER_PAPER (MANY_SUPPLIER_COUNT + 2)
#define MANY_SEM_SMOKER_GLUE (MANY_SUPPLIER_COUNT + 3)
#define MANY_SEM_COUNT (MANY_SUPPLIER_COUNT + 4)

enum {
    MANY_COMBO_TOBACCO_PAPER = 0, // wake glue smoker
    MANY_COMBO_TOBACCO_GLUE,      // wake paper smoker
    MANY_COMBO_PAPER_GLUE         // wake tobacco smoker
};

static volatile sig_atomic_t g_stop = 0;

union semun_local {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

static void on_signal(int signo)
{
    (void)signo;
    g_stop = 1;
}

static void install_signal_handlers(void)
{
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = on_signal;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}

static int sem_op(int semid, unsigned short sem_num, short op)
{
    struct sembuf sb;

    sb.sem_num = sem_num;
    sb.sem_op = op;
    sb.sem_flg = 0;

    while (semop(semid, &sb, 1) < 0) {
        if (errno == EINTR) {
            continue;
        }
        return -1;
    }
    return 0;
}

static int sem_down(int semid, unsigned short sem_num)
{
    return sem_op(semid, sem_num, -1);
}

static int sem_up(int semid, unsigned short sem_num)
{
    return sem_op(semid, sem_num, +1);
}

static int choose_combo(int step)
{
    return step % 3;
}

static unsigned short target_smoker_sem(int combo)
{
    switch (combo) {
    case MANY_COMBO_TOBACCO_PAPER:
        return MANY_SEM_SMOKER_GLUE;
    case MANY_COMBO_TOBACCO_GLUE:
        return MANY_SEM_SMOKER_PAPER;
    case MANY_COMBO_PAPER_GLUE:
        return MANY_SEM_SMOKER_TOBACCO;
    default:
        return MANY_SEM_SMOKER_TOBACCO;
    }
}

static const char *combo_text(int combo)
{
    switch (combo) {
    case MANY_COMBO_TOBACCO_PAPER:
        return "tobacco + paper";
    case MANY_COMBO_TOBACCO_GLUE:
        return "tobacco + glue";
    case MANY_COMBO_PAPER_GLUE:
        return "paper + glue";
    default:
        return "unknown";
    }
}

static const char *wakeup_text(int combo)
{
    switch (combo) {
    case MANY_COMBO_TOBACCO_PAPER:
        return "smoker_glue";
    case MANY_COMBO_TOBACCO_GLUE:
        return "smoker_paper";
    case MANY_COMBO_PAPER_GLUE:
        return "smoker_tobacco";
    default:
        return "unknown";
    }
}

static int init_sem_set(int semid)
{
    union semun_local arg;
    int i;

    for (i = 0; i < MANY_SUPPLIER_COUNT; i++) {
        arg.val = (i == 0) ? 1 : 0;
        if (semctl(semid, i, SETVAL, arg) < 0) {
            return -1;
        }
    }

    arg.val = 1;
    if (semctl(semid, MANY_SEM_TABLE_EMPTY, SETVAL, arg) < 0) {
        return -1;
    }

    arg.val = 0;
    if (semctl(semid, MANY_SEM_SMOKER_TOBACCO, SETVAL, arg) < 0) {
        return -1;
    }
    if (semctl(semid, MANY_SEM_SMOKER_PAPER, SETVAL, arg) < 0) {
        return -1;
    }
    if (semctl(semid, MANY_SEM_SMOKER_GLUE, SETVAL, arg) < 0) {
        return -1;
    }

    return 0;
}

static void supplier_loop(int semid, int supplier_id, int rounds)
{
    int done = 0;
    int step = supplier_id;
    unsigned short my_sem = (unsigned short)supplier_id;
    unsigned short next_sem = (unsigned short)((supplier_id + 1) % MANY_SUPPLIER_COUNT);

    install_signal_handlers();
    while (!g_stop && done < rounds) {
        int combo;
        unsigned short target;

        if (sem_down(semid, my_sem) < 0) {
            if (errno == EIDRM || errno == EINVAL) {
                break;
            }
            perror("supplier down turn");
            break;
        }
        if (sem_down(semid, MANY_SEM_TABLE_EMPTY) < 0) {
            if (errno == EIDRM || errno == EINVAL) {
                break;
            }
            perror("supplier down table");
            break;
        }

        combo = choose_combo(step++);
        target = target_smoker_sem(combo);
        printf("supplier_%d put %-16s -> wake %s\n", supplier_id, combo_text(combo), wakeup_text(combo));
        fflush(stdout);

        if (sem_up(semid, target) < 0) {
            perror("supplier wake smoker");
            break;
        }
        if (sem_up(semid, next_sem) < 0) {
            perror("supplier wake next supplier");
            break;
        }

        done++;
        usleep(100 * 1000);
    }
}

static void smoker_loop(int semid, int smoker_id)
{
    unsigned short my_sem;
    const char *own;

    switch (smoker_id % 3) {
    case 0:
        my_sem = MANY_SEM_SMOKER_TOBACCO;
        own = "tobacco";
        break;
    case 1:
        my_sem = MANY_SEM_SMOKER_PAPER;
        own = "paper";
        break;
    default:
        my_sem = MANY_SEM_SMOKER_GLUE;
        own = "glue";
        break;
    }

    install_signal_handlers();
    while (!g_stop) {
        if (sem_down(semid, my_sem) < 0) {
            if (errno == EIDRM || errno == EINVAL) {
                break;
            }
            perror("smoker down");
            break;
        }

        printf("smoker_%d(has %s): roll + smoke\n", smoker_id, own);
        fflush(stdout);
        usleep(200 * 1000);

        if (sem_up(semid, MANY_SEM_TABLE_EMPTY) < 0) {
            if (errno == EIDRM || errno == EINVAL) {
                break;
            }
            perror("smoker up table");
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    int rounds = 8;
    int semid;
    pid_t supplier_pids[MANY_SUPPLIER_COUNT];
    pid_t smoker_pids[MANY_SMOKER_COUNT];
    int i;

    if (argc > 1) {
        rounds = atoi(argv[1]);
        if (rounds <= 0) {
            rounds = 8;
        }
    }

    install_signal_handlers();

    for (i = 0; i < MANY_SUPPLIER_COUNT; i++) {
        supplier_pids[i] = -1;
    }
    for (i = 0; i < MANY_SMOKER_COUNT; i++) {
        smoker_pids[i] = -1;
    }

    semid = semget((key_t)MANY_SEM_KEY, MANY_SEM_COUNT, IPC_CREAT | 0666);
    if (semid < 0) {
        perror("semget");
        return 1;
    }
    if (init_sem_set(semid) < 0) {
        perror("init sem");
        semctl(semid, 0, IPC_RMID);
        return 1;
    }

    for (i = 0; i < MANY_SUPPLIER_COUNT; i++) {
        supplier_pids[i] = fork();
        if (supplier_pids[i] < 0) {
            perror("fork supplier");
            g_stop = 1;
            break;
        }
        if (supplier_pids[i] == 0) {
            supplier_loop(semid, i, rounds);
            _exit(0);
        }
    }

    for (i = 0; i < MANY_SMOKER_COUNT && !g_stop; i++) {
        smoker_pids[i] = fork();
        if (smoker_pids[i] < 0) {
            perror("fork smoker");
            g_stop = 1;
            break;
        }
        if (smoker_pids[i] == 0) {
            smoker_loop(semid, i);
            _exit(0);
        }
    }

    for (i = 0; i < MANY_SUPPLIER_COUNT; i++) {
        if (supplier_pids[i] > 0) {
            waitpid(supplier_pids[i], NULL, 0);
        }
    }

    for (i = 0; i < MANY_SMOKER_COUNT; i++) {
        if (smoker_pids[i] > 0) {
            kill(smoker_pids[i], SIGTERM);
        }
    }
    for (i = 0; i < MANY_SMOKER_COUNT; i++) {
        if (smoker_pids[i] > 0) {
            waitpid(smoker_pids[i], NULL, 0);
        }
    }

    if (semctl(semid, 0, IPC_RMID) < 0) {
        perror("semctl(IPC_RMID)");
        return 1;
    }

    printf("main_many: suppliers=%d smokers=%d cleaned up.\n", MANY_SUPPLIER_COUNT, MANY_SMOKER_COUNT);
    return 0;
}
