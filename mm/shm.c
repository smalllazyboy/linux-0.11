#include <sys/shm.h>
#include <linux/sched.h> // get_base()
#include <linux/mm.h>    // get_free_page() put_page()
#include <errno.h>       // errno signal

static int num = 0;
static shm_t shmList[shmNum] = {0};
// what is needed to be returned is virtual address,and the linear address acrroding to virtual address should be mapped to share memory
// 线性地址可以找到目录、页表，所以可以通过put_page，传入线性地址和物理地址，就能建立起两者的映射关系，也即是在相关页表中填上了共享内存的物理地址

int sys_shmget(key_t key, size_t size, int shmflg)
{
    int i;
    unsigned long tmp;
    for (i = 0; i < num; i++)
    {
        if (shmList[i].key == key)
            return shmList[i].shmid;
    }
    shmList[num].key = key;
    shmList[num].shmid = num;

    if (size >= 4096) // the unit is B
    {
        errno = EINVAL;
        return -1;
    }

    tmp = get_free_page();
    if (!tmp)
    {
        errno = ENOMEM;
        return -1;
    }
    shmList[num].page = tmp;

    return shmList[num++].shmid;
}
void *sys_shmat(int shmid, const void *shmaddr, int shmflg)
{
    int i;
    unsigned long tmp;
    for (i = 0; i < num; i++)
    {
        if (shmList[i].shmid == shmid)
        {
            // 得到当前进程的空闲虚拟内存地址（线性地址），后部分是代码数据段的基地址，前部分是breakpoint，数据段的结束地址
            tmp = current->brk + get_base(current->ldt[1]);
            // 映射线性地址和物理地址
            put_page(shmList[shmid].page, tmp);
            // 空闲虚拟内存减少
            current->brk += PAGE_SIZE;
            // 返回逻辑地址
            return current->brk - PAGE_SIZE;
        }
    }
    errno = EINVAL;
    return -1;
}