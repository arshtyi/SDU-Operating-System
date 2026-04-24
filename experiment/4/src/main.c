#include "smoker.h"

static volatile sig_atomic_t g_stop = 0;

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

static int parse_rounds(int argc, char *argv[])
{
    if (argc < 2) {
        return -1; // infinite
    }
    return atoi(argv[1]);
}

static void supplier_loop(const char *name, int semid, unsigned short my_sem, unsigned short next_sem, int start_step, int rounds)
{
    int done = 0;
    int step = start_step;

    install_signal_handlers();
    while (!g_stop && (rounds < 0 || done < rounds)) {
        int combo;
        unsigned short target;

        if (smoker_sem_down(semid, my_sem) < 0) {
            if (errno == EIDRM || errno == EINVAL) {
                break;
            }
            perror("supplier sem down");
            break;
        }
        if (smoker_sem_down(semid, SEM_TABLE_EMPTY) < 0) {
            if (errno == EIDRM || errno == EINVAL) {
                break;
            }
            perror("supplier table down");
            break;
        }

        combo = smoker_choose_combo(step++);
        target = smoker_target_sem(combo);

        printf("%s puts %-16s -> wake %s\n",
               name,
               smoker_combo_text(combo),
               smoker_name_for_combo(combo));
        fflush(stdout);

        if (smoker_sem_up(semid, target) < 0) {
            perror("supplier wake smoker");
            break;
        }
        if (smoker_sem_up(semid, next_sem) < 0) {
            perror("supplier wake next supplier");
            break;
        }

        done++;
        usleep(100 * 1000);
    }
}

static void smoker_loop(const char *name, const char *own_material, unsigned short sem_num, int semid)
{
    install_signal_handlers();
    while (!g_stop) {
        if (smoker_sem_down(semid, sem_num) < 0) {
            if (errno == EIDRM || errno == EINVAL) {
                break;
            }
            perror("smoker sem down");
            break;
        }

        printf("%s(has %s): roll + smoke\n", name, own_material);
        fflush(stdout);
        usleep(200 * 1000);

        if (smoker_sem_up(semid, SEM_TABLE_EMPTY) < 0) {
            if (errno == EIDRM || errno == EINVAL) {
                break;
            }
            perror("smoker table up");
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    int semid;
    int rounds;
    pid_t pids[5] = {-1, -1, -1, -1, -1};
    int i;

    install_signal_handlers();
    rounds = parse_rounds(argc, argv);

    semid = smoker_sem_get(1);
    if (semid < 0) {
        perror("semget");
        return 1;
    }
    if (smoker_sem_init(semid) < 0) {
        perror("sem init");
        smoker_sem_remove(semid);
        return 1;
    }

    pids[0] = fork();
    if (pids[0] < 0) {
        perror("fork supplier_a");
        smoker_sem_remove(semid);
        return 1;
    }
    if (pids[0] == 0) {
        supplier_loop("supplier_a", semid, SEM_SUPPLIER_A, SEM_SUPPLIER_B, 0, rounds);
        _exit(0);
    }

    pids[1] = fork();
    if (pids[1] < 0) {
        perror("fork supplier_b");
        g_stop = 1;
    }
    if (pids[1] == 0) {
        supplier_loop("supplier_b", semid, SEM_SUPPLIER_B, SEM_SUPPLIER_A, 1, rounds);
        _exit(0);
    }

    if (!g_stop) {
        pids[2] = fork();
        if (pids[2] < 0) {
            perror("fork smoker_tobacco");
            g_stop = 1;
        }
        if (pids[2] == 0) {
            smoker_loop("smoker_tobacco", "tobacco", SEM_SMOKER_TOBACCO, semid);
            _exit(0);
        }
    }

    if (!g_stop) {
        pids[3] = fork();
        if (pids[3] < 0) {
            perror("fork smoker_paper");
            g_stop = 1;
        }
        if (pids[3] == 0) {
            smoker_loop("smoker_paper", "paper", SEM_SMOKER_PAPER, semid);
            _exit(0);
        }
    }

    if (!g_stop) {
        pids[4] = fork();
        if (pids[4] < 0) {
            perror("fork smoker_glue");
            g_stop = 1;
        }
        if (pids[4] == 0) {
            smoker_loop("smoker_glue", "glue", SEM_SMOKER_GLUE, semid);
            _exit(0);
        }
    }

    if (!g_stop && rounds < 0) {
        while (!g_stop) {
            pause();
        }
    } else if (!g_stop) {
        waitpid(pids[0], NULL, 0);
        waitpid(pids[1], NULL, 0);
    }

    for (i = 0; i < 5; i++) {
        if (pids[i] > 0) {
            kill(pids[i], SIGTERM);
        }
    }
    for (i = 0; i < 5; i++) {
        if (pids[i] > 0) {
            waitpid(pids[i], NULL, 0);
        }
    }

    if (smoker_sem_remove(semid) < 0) {
        perror("sem remove");
        return 1;
    }

    printf("main: cleaned up semaphore set, exit.\n");
    return 0;
}
