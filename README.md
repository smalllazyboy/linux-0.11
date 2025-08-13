## Linux-0.11源码 ##
用于修改内核代码，熟悉操作系统，但是由于版本较老，高版本Ubuntu下载不了对应的gcc编译器，也或许有着其他不兼容问题，所以要么创建一个低版本Ubuntu虚拟机，要么就还是使用远程机操作https://www.lanqiao.cn/courses/115/learning/?id=570&compatibility=true。

### 关于信号量的实现与应用 ###
# 步骤
1.添加4个系统调用-->system_call.s,unistd.h,sys.h,semaphore.h,sem.c,Makefile
2.编写应用程序pc.c
3.在远程实验机中，由于linux-0.11是从/usr/include文件夹中的头文件编译.c文件，所以还需要将unistd.h、semaphore.h覆盖到该文件夹中才可以正确编译。