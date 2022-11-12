#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
int main(int argc, char const *argv[])
{

    if (argc != 2)
    {
        printf("a.out filename\n");
        return -1;
    }
    // int fd = open(argv[1], O_RDONLY);
    int64_t fd = 0;
  
    __asm__ volatile(
                     "movq $2, %%rax  \n\t" // open 系统函数编号
                     "movq %1, %%rdi  \n\t"
                     "movq %2, %%rsi  \n\t"
                     "movq $0, %%r10\n\t"
                     "syscall\n\t"
                     "movq %%rax,%0"
                     : "=r"(fd)
                     : "r"(argv[1]), "r"((uint64_t)O_RDONLY)
                     : "%rax", "%rdi", "%rsi", "%rdx","%r10");

    printf("fd:%ld\n",fd);
    if (fd <= 0)
    {
        
        perror("fd<=0");
        
        return -1;
    }

    char buf[513];
    int len = EOF;
    while ((len = read(fd, buf, 512)) > 0)
    {
        buf[len] = '\0';
        printf("%s", buf);
    }
    close(fd);

    return 0;
}
