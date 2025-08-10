## Linux-0.11源码 ##
用于修改内核代码，熟悉操作系统，但是由于版本较老，高版本Ubuntu下载不了对应的gcc编译器，也或许有着其他不兼容问题，所以要么创建一个低版本Ubuntu虚拟机，要么就还是使用远程机操作https://www.lanqiao.cn/courses/115/learning/?id=570&compatibility=true。


### 核心级进程切换 ###
1、switch_to、first_return_from_kernel要写在system_call.s中，通过.globl让全局可使用；ESP0、KERNEL_STACK两个宏定义也是写在system_call中；写两个函数的汇编代码的时候要注意格式（缩进）。
2、tss定义在sched.c中，在sched.h中通过extern让其他文件可以使用。
3、task_struct需要添加成员kernel_stack，在shecd.h中。
4、copy_process中要完成内核栈的压栈操作，需要根据switch_to、first_return_from_kernel、提示等内容按一定顺序将需要的数据压栈。
5、内核栈栈顶存储在PCB的kernel_stack中，也是在fork.c中完成，这是kernel_stack的初始化，切换则是在switch_to中。