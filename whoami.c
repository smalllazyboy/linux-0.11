/* 定义__LIBRARY__，使得_syscall2宏有效 */
#define __LIBRARY__

#include <stdio.h>
#include <errno.h>
#include "unistd.h"  /* 包含自定义系统调用编号 */

/* 使用_syscall2宏定义whoami系统调用接口 */
_syscall2(int, whoami, char*, name, unsigned int, size);

int main() {
    char name[24];
    int ret;
    
    ret = whoami(name, sizeof(name));
    
    if (ret == -1) {
        perror("whoami 系统调用失败");
        return 1;
    }
    
    printf("当前设置的名字: %s\n", name);
    printf("名字长度: %d\n", ret);
    return 0;
}
    