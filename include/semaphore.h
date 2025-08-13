#define __LIBRARY__
#include <unistd.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#include <asm/system.h>
#include <linux/sched.h>

#define NAME_SIZE 40
#define SEM_NUM 40

typedef struct
{
    char name[NAME_SIZE];
    int value;
    struct task_struct *queue;
} sem_t;

_syscall2(sem_t *, sem_open, const char *, name, unsigned int, value);
_syscall1(int, sem_wait, sem_t *, sem);
_syscall1(int, sem_post, sem_t *, sem);
_syscall1(int, sem_unlink, const char *, name);