#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>

#define HZ 100

void cpuio_bound(int last, int cpu_time, int io_time);

void show_einval_massage(void)
{
    puts("This sample program needs arguments.");
    puts("Please input two numbers after the file name of this program.");
    puts("The format is \"./process last n\",");
    puts("where last is the time a sub process will last for");
    puts("and n is the number of sub processes,");
    puts("and n must be the divisor of last.");
    puts("Example: input \"./process 24 6");
    puts("Then each sub process will last for 24 seconds");
    puts("and there will be 6 processes.");
}

int main(int argc, char *argv[])
{
    int last, n, d, pid, j, x, t;
    if (argc != 3)
    {
        show_einval_massage();
        return 22;
    }
    else
    {
        sscanf(argv[1], "%d", &last);
        sscanf(argv[2], "%d", &n);
        if (last % n == 0)
        {
            d = last / n;
            printf("This is process %d.\n", getpid());
            for (j = 0; j < n; j++)
            {
                pid = fork();
                if (pid == 0)
                {
                    cpuio_bound(last, (n - j) * d, d);
                    return 0;
                }
                else
                    printf("Sub process %d is created!\n", pid);
            }
            for (j = 0; j < n; j++)
            {
                t = wait(&x);
                printf("Sub process %d finished with exit code %d\n", t, x);
            }
        }
        else
        {
            show_einval_massage();
            return 22;
        }
    }
    return 0;
}

void cpuio_bound(int last, int cpu_time, int io_time)
{
    struct tms start_time, current_time;
    clock_t utime, stime;
    int sleep_time;

    while (last > 0)
    {
        times(&start_time);

        do
        {
            times(&current_time);
            utime = current_time.tms_utime - start_time.tms_utime;
            stime = current_time.tms_stime - start_time.tms_stime;
        } while (((utime + stime) / HZ) < cpu_time);
        last -= cpu_time;

        if (last <= 0)
            break;

        sleep(io_time);
    }
}