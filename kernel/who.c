#include <linux/kernel.h>
#include <linux/sched.h>
#include <string.h>
#include <errno.h>
#include <asm/segment.h>

// 内核中存储名字的全局变量，最多23个字符 + 终止符
static char kernel_name[24] = {0};

// iam() 系统调用实现
int sys_iam(const char *name)
{
    int len = 0;
    char tmp[24]; // 临时缓冲区
    int i;        // 在循环外声明变量

    // 从用户空间复制字符串到内核缓冲区
    while (len < 24)
    {
        // 使用 get_fs_byte 安全读取用户空间数据
        tmp[len] = get_fs_byte(name + len);
        if (tmp[len] == '\0')
            break;
        len++;
    }

    // 检查名字长度是否超过23个字符
    if (len >= 24)
    {
        errno = EINVAL;
        return -1;
    }

    // 复制到内核全局变量
    strcpy(kernel_name, tmp);

    return len;
}

// whoami() 系统调用实现
int sys_whoami(char *name, unsigned int size)
{
    int len = strlen(kernel_name);
    int i; // 在循环外声明变量，解决C99模式问题

    // 检查缓冲区大小是否足够
    if (size <= len)
    {
        errno = EINVAL;
        return -1;
    }

    // 将内核中的名字复制到用户空间
    for (i = 0; i <= len; i++)
    { // 使用已声明的i
        // 使用 put_fs_byte 安全写入用户空间
        put_fs_byte(kernel_name[i], name + i);
    }

    return len;
}
