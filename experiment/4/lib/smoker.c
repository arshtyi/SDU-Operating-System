#include "smoker.h"

union semun_local {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

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

int smoker_sem_get(int create)
{
    int flags = 0666;
    if (create) {
        flags |= IPC_CREAT;
    }
    return semget((key_t)SMOKER_SEM_KEY, SEM_COUNT, flags);
}

int smoker_sem_init(int semid)
{
    union semun_local arg;

    arg.val = 1;
    if (semctl(semid, SEM_SUPPLIER_A, SETVAL, arg) < 0) {
        return -1;
    }

    arg.val = 0;
    if (semctl(semid, SEM_SUPPLIER_B, SETVAL, arg) < 0) {
        return -1;
    }

    arg.val = 1;
    if (semctl(semid, SEM_TABLE_EMPTY, SETVAL, arg) < 0) {
        return -1;
    }

    arg.val = 0;
    if (semctl(semid, SEM_SMOKER_TOBACCO, SETVAL, arg) < 0) {
        return -1;
    }
    if (semctl(semid, SEM_SMOKER_PAPER, SETVAL, arg) < 0) {
        return -1;
    }
    if (semctl(semid, SEM_SMOKER_GLUE, SETVAL, arg) < 0) {
        return -1;
    }

    return 0;
}

int smoker_sem_remove(int semid)
{
    return semctl(semid, 0, IPC_RMID);
}

int smoker_sem_down(int semid, unsigned short sem_num)
{
    return sem_op(semid, sem_num, -1);
}

int smoker_sem_up(int semid, unsigned short sem_num)
{
    return sem_op(semid, sem_num, +1);
}

int smoker_choose_combo(int step)
{
    return step % 3;
}

unsigned short smoker_target_sem(int combo)
{
    switch (combo) {
    case COMBO_TOBACCO_PAPER:
        return SEM_SMOKER_GLUE;
    case COMBO_TOBACCO_GLUE:
        return SEM_SMOKER_PAPER;
    case COMBO_PAPER_GLUE:
        return SEM_SMOKER_TOBACCO;
    default:
        return SEM_SMOKER_TOBACCO;
    }
}

const char *smoker_combo_text(int combo)
{
    switch (combo) {
    case COMBO_TOBACCO_PAPER:
        return "tobacco + paper";
    case COMBO_TOBACCO_GLUE:
        return "tobacco + glue";
    case COMBO_PAPER_GLUE:
        return "paper + glue";
    default:
        return "unknown";
    }
}

const char *smoker_name_for_combo(int combo)
{
    switch (combo) {
    case COMBO_TOBACCO_PAPER:
        return "smoker_glue";
    case COMBO_TOBACCO_GLUE:
        return "smoker_paper";
    case COMBO_PAPER_GLUE:
        return "smoker_tobacco";
    default:
        return "unknown";
    }
}
