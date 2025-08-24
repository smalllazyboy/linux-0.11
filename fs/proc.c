#include <sys/types.h>
#include <errno.h>
#include <stdarg.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <string.h>
#include <asm/segment.h>

static char *psinfo_buf;
static int length;

int sprintf(char *buf, const char *fmt, ...)
{
    va_list args;
    int i;
    va_start(args, fmt);
    i = vsprintf(buf, fmt, args);
    va_end(args);
    return i;
}
// proc 文件的处理函数的功能是根据设备编号，把不同的内容写入到用户空间的 buf。
// 写入的数据要从 f_pos 指向的位置开始，每次最多写 count 个字节，
// 并根据实际写入的字节数调整 f_pos 的值，最后返回实际写入的字节数。
// 当设备编号表明要读的是 psinfo 的内容时，就要按照 psinfo 的形式组织数据。

// inode->i_zone[0] ，这就是 mknod() 时指定的 dev ——设备编号
// buf，指向用户空间，就是 read() 的第二个参数，用来接收数据
// count，就是 read() 的第三个参数，说明 buf 指向的缓冲区大小
/* & file -> f_pos，f_pos 是上一次读文件结束时“文件位置指针”的指向。
这里必须传指针，因为处理函数需要根据传给 buf 的数据量修改 f_pos 的值。 */
int psinfo(char *buf, int count, off_t *pos);

int proc_file_read(int dev, char *buf, int count, off_t *pos)
{
    switch (dev)
    {
    case 0:
        return psinfo(buf, count, pos);
    }
    return -EINVAL;
}
// 需要打印进程信息，所以需要访问全局结构体task，找到各个进程的相关信息
// --- 输出格式 ---
// pid    state    father    counter    start_time
// 0    1    -1    0    0
// 1    1    0    28    1
// 4    1    1    1    73
// 3    1    1    27    63
// 6    0    4    12    817
int psinfo(char *buf, int count, off_t *pos)
{
    // 思路：申请内存存储全部数据（在最后写上\0），再从内存中读出到buf。因为读出到buf的内容有限制
    /* 在open中会将f_pos置为0，也就是每次打开都会从文件的开头开始读。而f_pos的作用是记
    录上一次读文件的位置，这个应该牵扯到了 buf有所限制 ，buf应该是一个有一定大小的环形缓冲区，所以
    如果读到buf的时候buf不够用了，相当于就是count为0了，就会先把这次读到的内容先打印，再调整buf到
    开头，再次读，此时f_pos就起到了作用。所以在proc的处理函数中，应该要把所有内容先放到一个内存中，
    然后再从这个内存中根据相同的逻辑（考虑count）读到buf，而f_pos就记录上一次读到的位置。这样的话，
    在buf不够用的时候，在对应的机制（cat的实现原理）下，会循环调用该处理函数直到读到\0），并从f_pos记录的位置开始读。*/
    // 所以说该思路是在该机制下所被要求的
    int chars, i;
    char *my_buf;
    struct task_struct **p;

    if (*pos == 0)
    {
        // 首先申请一个能容纳所有进程信息的内存 37（tag）+NR_TASKS * 5 * 4 = 1317B
        my_buf = psinfo_buf = (char *)malloc(strlen("pid\tstate\tfather\tcounter\tstart_time\n\r") + NR_TASKS * 5 * sizeof(long) + 1);
        // 向psinfo_buf中放入tags
        chars = sprintf(psinfo_buf, "pid\tstate\tfather\tcounter\tstart_time\n");
        // chars--;
        // 获取进程相关信息放入mu_buf
        for (p = &LAST_TASK; p >= &FIRST_TASK; --p)
        {
            if (*p)
            {
                chars += sprintf(&psinfo_buf[chars], "%ld\t%ld\t%ld\t%ld\t%ld\n", (*p)->pid, (*p)->state, (*p)->father, (*p)->counter, (*p)->start_time);
            }
        }
        put_fs_byte('\0', psinfo_buf + chars);
        length = chars;
    }
    // 将psinfo_buf的内容读到buf
    my_buf = psinfo_buf + *pos;
    chars = length - *pos;
    if (chars > count)
        chars = count;
    i = chars;
    while (i-- > 0)
    {
        put_fs_byte(*my_buf, buf++);
        my_buf++;
    }
    *pos += chars;
    if (chars == 0)
    {
        free(psinfo_buf);
    }
    return chars;
}