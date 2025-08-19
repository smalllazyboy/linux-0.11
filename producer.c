#include <stdio.h>
#include <stdlib.h>
// #include <unistd.h>  说是会与semaphore.h中的unistd.h冲突？？？
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <fcntl.h>

#define PRODUCE_NUM 800
#define BUFF_SIZE 10
#define KEY 2

int main(int argn, char *argc[])
{
    sem_t *semEmpty, *semFull, *semMutex;
    int i, shmid, current = 0;
    int *arr;

    sem_unlink("Mutex");
    sem_unlink("Full");
    sem_unlink("Empty");

    semEmpty = sem_open("Empty", BUFF_SIZE);
    semFull = sem_open("Full", 0);
    semMutex = sem_open("Mutex", 1);
    printf("semEmpty's address in producer is %p\n", &semEmpty);
    printf("semEmpty in producer is %p\n", semEmpty);

    // 在共享内存中写入10个初始值
    shmid = shmget(KEY, BUFF_SIZE * sizeof(int), 0);
    arr = (int *)shmat(shmid, NULL, 0);
    printf("shmget is successful in producer\n");
    fflush(stdout);
    for (i = 0; i < BUFF_SIZE; i++)
    {
        arr[i] = 0;
    }

    // 写入的数字
    for (i = 1; i <= PRODUCE_NUM; i++)
    {
        sem_wait(semEmpty);
        sem_wait(semMutex);
        arr[current] = i;
        printf("producer produce %d on %p\n", i, &arr[current]);
        fflush(stdout); // 同步进程间stdout的目前位置，确保写的内容不会相互覆盖
        current++;
        current %= BUFF_SIZE; // 确保目前取数位置正确
        sem_post(semFull);
        sem_post(semMutex);
    }

    return 0;
}