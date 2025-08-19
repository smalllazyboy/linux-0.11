#define __LIBRARY__
#include <unistd.h> // _syscall
#include <stddef.h> // size_t
// #include <linux/kernel.h>
// 还需要包含在shm.c中需要的头文件

typedef int key_t;
typedef struct
{
    key_t key;
    int shmid;
    void *page;
} shm_t;
#define shmNum 40

_syscall3(int, shmget, key_t, key, size_t, size, int, shmflg);
_syscall3(void *, shmat, int, shmid, const void *, shmaddr, int, shmflg);
