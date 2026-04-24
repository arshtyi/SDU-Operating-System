/*
 * Filename: consumer.c
 * copyright: (C) by zhanghonglie
 * Function: 建立并模拟消费者进程
 */
#include "ipc.h"

int main(int argc, char *argv[])
{
    int rate;

    // 可在命令行第一参数指定一个进程睡眠秒数，以调节进程执行速度
    if (argc > 1 && argv[1] != NULL) {
        rate = atoi(argv[1]);
    } else {
        rate = 3;
    }

    // 共享内存使用的变量
    buff_key = 101; // 缓冲区任给的键值
    buff_num = 8;   // 缓冲区任给的长度
    cget_key = 103; // 消费者取产品指针的键值
    cget_num = 1;   // 指针数
    shm_flg = IPC_CREAT | 0644;

    // 获取缓冲区和消费者取产品指针
    buff_ptr = set_shm(buff_key, buff_num, shm_flg);
    cget_ptr = (int *)set_shm(cget_key, cget_num, shm_flg);

    // 信号量使用的变量
    prod_key = 201; // 生产者同步信号量键值
    pmtx_key = 202; // 生产者互斥信号量键值
    cons_key = 301; // 消费者同步信号量键值
    cmtx_key = 302; // 消费者互斥信号量键值
    sem_flg = IPC_CREAT | 0644;

    sem_val = buff_num;
    prod_sem = set_sem(prod_key, sem_val, sem_flg);

    sem_val = 0;
    cons_sem = set_sem(cons_key, sem_val, sem_flg);

    sem_val = 1;
    cmtx_sem = set_sem(cmtx_key, sem_val, sem_flg);

    // 循环执行模拟消费者不断取产品
    while (1) {
        down(cons_sem); // 如果无产品则阻塞
        down(cmtx_sem); // 如果有其他消费者正在取则阻塞

        sleep(rate);
        printf("%d consumer get: %c from Buffer[%d]\n", getpid(), buff_ptr[*cget_ptr], *cget_ptr);

        *cget_ptr = (*cget_ptr + 1) % buff_num;

        up(cmtx_sem); // 唤醒阻塞的消费者
        up(prod_sem); // 唤醒阻塞的生产者
    }

    return EXIT_SUCCESS;
}
