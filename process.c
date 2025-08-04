#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <stdlib.h>
#define HZ 100

// 必须提前声明函数（C89 要求）
void cpuio_bound(int last, int cpu_time, int io_time);

int main(int argc, char *argv[])
{
    // C89 要求：所有变量必须在函数最开头声明
    pid_t pid;
    int i;
    int status;
    int children_completed = 0;
    // 数组初始化必须按 C89 格式（显式写全）
    int tasks[5][3] = {
        {10, 1, 0},
        {10, 0, 1},
        {20, 2, 1},
        {20, 1, 2},
        {15, 3, 3}};

    for (i = 0; i < 5; i++)
    {
        pid = fork();
        if (pid < 0)
        {
            perror("fork failed");
            exit(1);
        }
        else if (pid == 0)
        {
            // printf 参数必须严格匹配（C89 对可变参数检查更严）
            printf("Child process %d started (PID: %d)\n", i + 1, getpid());
            cpuio_bound(tasks[i][0], tasks[i][1], tasks[i][2]);
            printf("Child process %d completed (PID: %d)\n", i + 1, getpid());
            exit(0);
        }
    }

    // 等待子进程
    while (children_completed < 5)
    {
        pid_t exited_pid = wait(&status);
        if (exited_pid > 0)
        {
            children_completed++;
            printf("Parent process: Child process %d has exited\n", exited_pid);
        }
    }

    printf("All child processes have completed, parent process exiting\n");
    return 0;
} // 闭合 main 函数

// 函数定义必须严格匹配声明，补全大括号
void cpuio_bound(int last, int cpu_time, int io_time)
{
    struct tms start_time, current_time;
    clock_t my_utime, my_stime; // 重命名避免与库函数冲突
    int sleep_time;

    while (last > 0)
    {
        times(&start_time);
        do
        {
            times(&current_time);
            my_utime = current_time.tms_utime - start_time.tms_utime;
            my_stime = current_time.tms_stime - start_time.tms_stime;
        } while ((my_utime + my_stime) / HZ < cpu_time);

        last -= cpu_time;
        if (last <= 0)
            break;

        // sleep 必须确保头文件正确（<unistd.h> 已包含）
        sleep_time = 0;
        while (sleep_time < io_time)
        {
            sleep(1);
            sleep_time++;
        }
        last -= sleep_time;
    }
} // 闭合 cpuio_bound 函数