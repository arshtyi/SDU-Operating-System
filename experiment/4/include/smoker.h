#ifndef SMOKER_H
#define SMOKER_H

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

#define SMOKER_SEM_KEY 0x55A1

enum {
    SEM_SUPPLIER_A = 0,
    SEM_SUPPLIER_B,
    SEM_TABLE_EMPTY,
    SEM_SMOKER_TOBACCO,
    SEM_SMOKER_PAPER,
    SEM_SMOKER_GLUE,
    SEM_COUNT
};

enum {
    COMBO_TOBACCO_PAPER = 0, // wake smoker with glue
    COMBO_TOBACCO_GLUE,      // wake smoker with paper
    COMBO_PAPER_GLUE         // wake smoker with tobacco
};

int smoker_sem_get(int create);
int smoker_sem_init(int semid);
int smoker_sem_remove(int semid);
int smoker_sem_down(int semid, unsigned short sem_num);
int smoker_sem_up(int semid, unsigned short sem_num);

int smoker_choose_combo(int step);
unsigned short smoker_target_sem(int combo);
const char *smoker_combo_text(int combo);
const char *smoker_name_for_combo(int combo);

#endif
