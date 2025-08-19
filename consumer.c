#include <stdio.h>
#include <semaphore.h>
#include <sys/shm.h>

#define PRODUCE_NUM 800
#define BUFF_SIZE 10
#define KEY 2

int main(int argn, char *argc[])
{
    sem_t *semEmpty, *semFull, *semMutex;
    int i, shmid, current = 0;
    int *arr;

    semEmpty = sem_open("Empty", BUFF_SIZE);
    semFull = sem_open("Full", 0);
    semMutex = sem_open("Mutex", 1);
    printf("semEmpty's address in consumer is %p\n", &semEmpty);
    printf("semEmpty in consumer is %p\n", semEmpty); // 1e660

    // 获取共享内存
    shmid = shmget(KEY, BUFF_SIZE * sizeof(int), 0);
    arr = (int *)shmat(shmid, NULL, 0);
    printf("shmget is successful in consumer\n");
    fflush(stdout);

    // 消费数字
    for (i = 0; i < PRODUCE_NUM; i++)
    {
        sem_wait(semFull);
        sem_wait(semMutex);
        printf("consumer consume %d on %p\n", arr[current], &arr[current]); // 5000 - 5024 一共40字节=4*10
        current++;
        current %= BUFF_SIZE; // 确保目前取数位置正确
        sem_post(semEmpty);
        sem_post(semMutex);
    }

    // 由于consumer的滞后性，将unlink将给consumer
    sem_unlink("Mutex");
    sem_unlink("Full");
    sem_unlink("Empty");
    while (1)
        ;
    return 0;
}