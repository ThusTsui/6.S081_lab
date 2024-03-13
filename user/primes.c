#include "kernel/types.h"
#include "user/user.h"

#define RD 0
#define WR 1

// 读取左邻居第一个数据,有数据返回0
int lpipe_first_data(int lpipe[2], int *dst)
{
    if (read(lpipe[RD], dst, sizeof(int)) == sizeof(int))
    {
        printf("prime %d\n", *dst);
        return 0;
    }
    return -1;
}

// 读取左邻居的数据，将不能被first整除的写进右邻居
void transmit_data(int lpipe[2], int rpipe[2], int first)
{
    int data;
    // 从左管道读取数据
    while (read(lpipe[RD], &data, sizeof(int)) == sizeof(int))
    {
        // 将无法整除的数据传递入右管道
        if (data % first)
            write(rpipe[WR], &data, sizeof(int));
    }
    close(lpipe[RD]);
    close(rpipe[WR]);
}

// 该进程通过一个管道从其左邻居读取数据，
// 并通过另一个管道向其右邻居写入数据。
void primes(int lpipe[2])
{
    close(lpipe[WR]); // 左邻居只读
    int first;
    if (lpipe_first_data(lpipe, &first) == 0) // 是素数
    {
        int p[2];
        pipe(p); // 当前的管道
        transmit_data(lpipe, p, first);

        if (fork() == 0)
        {
            primes(p); // 递归的思想，但这将在一个新的进程中调用
        }
        else
        {
            close(p[RD]);
            wait(0);
        }
    }
    exit(0);
}

int main(int argc, char const *argv[])
{
    int p[2];
    pipe(p);

    for (int i = 2; i <= 35; ++i) // 写入初始数据
    {
        write(p[WR], &i, sizeof(int));
    }
    if (fork() == 0)
    {
        primes(p);
    }
    else
    {
        close(p[WR]);
        close(p[RD]);
        wait(0); // 等待子进程exit(0)
    }
    exit(0);
}