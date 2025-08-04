## Linux-0.11 ##
### RTFSC ###
Linus曾经说过：RTFSC - Read The Fucking Source Code.

本代码是目前能够找到的最早的Linux的内核版本。（如果你能找到更早版本的Linux源码，请一定要告诉[我](karottc@gmail.com)。;-) ）

本代码中的注释99%都来源于赵炯老师的那本[Linux-0.11源码完全注释](http://book.douban.com/subject/1231236/)，这么算的话，我好像只是一个搬运工....（不过搬运的过程经过了自己的手，总能得到一些东西... :-) ）

##### 附注 #####
目前更新到kernel部分的注释，驱动部分应该不会更新，目前感兴趣的部分在kernle、内存管理、网络(这个版本还没有网络....). 就酱 ! ! !

### ////////////////// 系统调用 ////////////////// ###
# 实验地址：https://www.lanqiao.cn/courses/115/learning/?id=569&compatibility=false
除了运用整个代码之外，还需要将自定义系统调用的宏添加到0.11环境下的/usr/include/unistd.h中，该文件是0.11环境下编译C程序的标准头文件
# 添加文件说明
iam.c和whoami.c是需要放在0.11环境下编译运行的测试程序