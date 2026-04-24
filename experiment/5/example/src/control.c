/**
 * Filename: control.c
 * copyright: (C) 2006 by zhonghonglie
 * Function: 建立并模拟控制者进程
 */
#include "ipc.h"
int main(int argc, char* argv[]) {
    int i;
    int active_readers = 0;
    int writer_waiting = 0;
    int writer_mid = -1;
    Msg_buf msg_arg;
    // 建立一个共享内存先写入一串 A 字符模拟要读写的内容
    buff_key = 101;
    buff_num = STRSIZ + 1;
    shm_flg = IPC_CREAT | 0644;
    buff_ptr = (char*)set_shm(buff_key, buff_num, shm_flg);
    for (i = 0; i < STRSIZ; i++)
        buff_ptr[i] = 'A';
    buff_ptr[i] = '\0';
    // 建立一条请求消息队列
    quest_flg = IPC_CREAT | 0644;
    quest_key = 201;
    quest_id = set_msq(quest_key, quest_flg);
    // 建立一条响应消息队列
    respond_flg = IPC_CREAT | 0644;
    respond_key = 202;
    respond_id = set_msq(respond_key, respond_flg);
    // 控制进程准备接收和响应读写者的消息
    printf("Wait quest \n");
    while (1) {
        quest_flg = IPC_NOWAIT; // 非阻塞轮询请求队列

        // 优先处理完成消息，尽快更新当前活动读者数
        if (msgrcv(quest_id, &msg_arg, sizeof(msg_arg), FINISHED, quest_flg) >= 0) {
            if (active_readers > 0) {
                active_readers--;
            }
            printf("%d reader finished\n", msg_arg.mid);
            continue;
        }

        // 如已有写者等待且当前无读者，先放行写者（写者优先）
        if (writer_waiting && active_readers == 0) {
            msg_arg.mid = writer_mid;
            msg_arg.mtype = writer_mid;
            msgsnd(respond_id, &msg_arg, sizeof(msg_arg), 0);
            printf("%d quest write\n", writer_mid);

            // 阻塞等待该写者完成
            msgrcv(quest_id, &msg_arg, sizeof(msg_arg), FINISHED, 0);
            printf("%d write finished\n", msg_arg.mid);
            writer_waiting = 0;
            writer_mid = -1;

            // 写完后一次性放行当前排队读者
            while (msgrcv(quest_id, &msg_arg, sizeof(msg_arg), READERQUEST, IPC_NOWAIT) >= 0) {
                active_readers++;
                msg_arg.mtype = msg_arg.mid;
                msgsnd(respond_id, &msg_arg, sizeof(msg_arg), 0);
                printf("%d quest read\n", msg_arg.mid);
            }
            continue;
        }

        // 没有待处理写者时，接收新的写请求
        if (!writer_waiting && msgrcv(quest_id, &msg_arg, sizeof(msg_arg), WRITERQUEST, quest_flg) >= 0) {
            writer_waiting = 1;
            writer_mid = msg_arg.mid;
            printf("%d writer quest\n", writer_mid);
            continue;
        }

        // 只有在没有写者等待时才允许新读者进入
        if (!writer_waiting && msgrcv(quest_id, &msg_arg, sizeof(msg_arg), READERQUEST, quest_flg) >= 0) {
            active_readers++;
            msg_arg.mtype = msg_arg.mid;
            msgsnd(respond_id, &msg_arg, sizeof(msg_arg), 0);
            printf("%d quest read\n", msg_arg.mid);
        }
    }
    return EXIT_SUCCESS;
}
