#include <stdio.h>
#include <stdlib.h>
// #include <unistd.h>  说是会与semaphore.h中的unistd.h冲突？？？
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#define PRODUCE_NUM 800
#define CONSUMER_NUM 5
#define BUFF_SIZE 10

void die(const char *p)
{
    puts(p);
    exit(1);
}

void producer(sem_t *Empty, sem_t *Full, sem_t *Mutex, int fd)
{
    int i, location = 0;
    for (i = 0; i < PRODUCE_NUM; i++)
    {
        sem_wait(Empty);
        sem_wait(Mutex);
        if (lseek(fd, sizeof(int) * location, SEEK_SET) == -1)
            die("seek error");
        if (write(fd, &i, sizeof(int)) == -1)
            die("error write file");
        sem_post(Full);
        sem_post(Mutex);
        location = (location + 1) % 10;
    }
    exit(0);
}

void consumer(sem_t *Empty, sem_t *Full, sem_t *Mutex, int fd)
{
    int i, pro, pid, location;
    pid = getpid();
    for (i = 0; i < PRODUCE_NUM / CONSUMER_NUM; i++)
    {
        sem_wait(Full);
        sem_wait(Mutex);
        if (lseek(fd, sizeof(int) * -1, SEEK_END) == -1)
            die("seek error");
        if (read(fd, &location, sizeof(int)) == -1)
            die("read error");
        if (lseek(fd, sizeof(int) * location, SEEK_SET) == -1)
            die("seek error");
        if (read(fd, &pro, sizeof(int)) == -1)
            die("read error");
        if (lseek(fd, sizeof(int) * -1, SEEK_END) == -1)
            die("seek error");
        location = (location + 1) % 10;
        if (write(fd, &location, sizeof(int)) == -1)
            die("writerror");
        printf("%d:%d\n", pid, pro);
        fflush(stdout); // 同步进程间stdout的目前位置，确保写的内容不会相互覆盖
        sem_post(Empty);
        sem_post(Mutex);
    }
    exit(0);
}

int main(int argn, char *argc[])
{
    sem_t *semEmpty, *semFull, *semMutex;
    int i, fd, x = 0;

    semEmpty = sem_open("Empty", BUFF_SIZE);
    semFull = sem_open("Full", 0);
    semMutex = sem_open("Mutex", 1);
    if ((fd = open("./container", O_RDWR | O_CREAT, 0600)) == -1)
        die("error of open file");
    printf("Open file successed\n");

    // 写入11个初始值0，最后一个表示目前取数的位置
    for (i = 0; i < BUFF_SIZE + 1; i++)
        if (write(fd, &x, sizeof(int)) == -1)
            die("error write file");

    if (!fork())
        producer(semEmpty, semFull, semMutex, fd);
    for (i = 0; i < CONSUMER_NUM; i++)
        if (!fork())
            consumer(semEmpty, semFull, semMutex, fd);
    for (i = 0; i < CONSUMER_NUM + 1; i++)
    {
        wait(NULL);
    }
    if (close(fd) == -1)
        die("close error");
    printf("close succeed\n");
    sem_unlink("Mutex");
    sem_unlink("Full");
    sem_unlink("Empty");
    return 0;
}