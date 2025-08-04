/* 定义__LIBRARY__，使得_syscall1宏有效 */
#define __LIBRARY__

#include <stdio.h>
#include <errno.h>
#include "unistd.h"  /* 包含自定义系统调用编号 */

/* 使用_syscall1宏定义iam系统调用接口 */
_syscall1(int, iam, const char*, name);

int main(int argc, char *argv[]) {
    int ret;
    
    if (argc < 2) {
        fprintf(stderr, "用法: %s <名字>\n", argv[0]);
        return 1;
    }
    
    ret = iam(argv[1]);
    
    if (ret == -1) {
        perror("iam 系统调用失败");
        return 1;
    }
    
    printf("成功设置名字，长度: %d\n", ret);
    return 0;
}
    