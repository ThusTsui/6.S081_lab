#include "kernel/types.h"
#include "user/user.h"

/*
使用两个管道，每个方向一个实现
父->子，子打印后退出；
子->父，父打印后退出。

使用两个管道进行父子进程通信，
需要注意的是如果管道的写端没有`close`，
那么管道中数据为空时对管道的读取将会阻塞。
因此对于不需要的管道描述符，要尽可能早的关闭。
*/

#define R 0 // pipe的read端
#define W 1 // pipe的write端

int main(int argc, char const *argv[])
{
    char buf = 'P'; // 用于传送的字节

    int fd_c2p[2]; // 子进程->父进程
    int fd_p2c[2]; // 父进程->子进程
    pipe(fd_c2p);
    pipe(fd_p2c);

    int pid = fork();

    if (pid < 0) // 出错，全部关闭退出
    {
        fprintf(2, "fork() error!\n");
        close(fd_c2p[R]);
        close(fd_c2p[W]);
        close(fd_p2c[R]);
        close(fd_p2c[W]);
        exit(1);
    }
    else if (pid == 0)
    {
        close(fd_c2p[R]);
        close(fd_p2c[W]);

        read(fd_p2c[R], &buf, 1);
        printf("%d: received ping\n", getpid());
        write(fd_c2p[W], &buf, 1);

        close(fd_p2c[R]);
        close(fd_c2p[W]);
        exit(0);
    }
    else//父进程
    {
        close(fd_p2c[R]);
        close(fd_c2p[W]);
        write(fd_p2c[W], &buf, 1);
        read(fd_c2p[R],&buf,1);
        printf("%d: received pong\n", getpid());
        

        close(fd_c2p[R]);
        close(fd_p2c[W]);
        exit(0);
    }
}

//一定要有一个进程先写，不然都read会死锁