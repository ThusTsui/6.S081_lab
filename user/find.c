#include "kernel/types.h"

#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

void find(char *path, const char *filename)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;
    /*
    dirent 在目录流中读取目录,包含了目录中文件的相关信息，比如文件名等。
    stat 获取文件状态信息，可以获取文件的详细属性，比如文件大小、权限等。
    这两个结构体通常在操作文件和目录时会用到，其中 dirent 用于读取目录中的文件列表，而 stat 用于获取文件的状态信息。*/
    if ((fd = open(path, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot fstat %s\n", path);
        close(fd);
        return;
    }
    // 参数错误，find的第一个参数必须是目录
    if (st.type != T_DIR)
    {
        fprintf(2, "usage: find <DIRECTORY> <filename>\n");
        return;
    }

    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
    {
        fprintf(2, "find: path too long\n");
        return;
    }

    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/'; // p指向最后一个'/'之后
    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
        if (de.inum == 0) // 目录不存在
            continue;
        memmove(p, de.name, DIRSIZ); // 添加路径名称
        p[DIRSIZ] = 0;               // 字符串结束标志
        if (stat(buf, &st) < 0)
        {
            fprintf(2, "find: cannot stat %s\n", buf);
            continue;
        }
        if (st.type == T_DIR && strcmp(p, ".") != 0 && strcmp(p, "..") != 0)
        {
            find(buf, filename);
        }
        else if (strcmp(filename, p) == 0)
            printf("%s\n", buf);
    }

    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(2, "Usage: find [path] [filename]\n");
        exit(-1);
    }
    find(argv[1], argv[2]);
    exit(0);
}