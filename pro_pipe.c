//管道编程
//在父子进程间通过两个管道进行双向通讯。父进程从标准输入输入文本数据，
//通过管道1将数据发往子进程，子进程将所有小写字母，通过管道2将加工后的数据发回给父进程，
//父进程将收到的数据输出到标准输出，重复上述过程，直到输入了自己指定的结束字符串为止。
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <ctype.h>

void process(int *fd1,int *fd2)
{
    if(fork()>0)
    {
        close(fd1[0]);
        close(fd2[1]);
        while(1)
        {
            char buffer[128]={0};
            //此处堵塞
            fgets(buffer,128,stdin);  //从键盘读取一定的字符
            write(fd1[1],buffer,strlen(buffer));//往管道的一端写
            if(buffer[0]=='-'&&buffer[1]=='1')
            break;
            memset(buffer,0,128);
            //读取从子进程传过来的文件，可能堵塞
            read(fd2[0],buffer,128);
            printf("从子进程传来的是%s\n",buffer);
        }
    }
    //子进程
    else
    {
        close(fd1[1]);
        close(fd2[0]);
        while(1)
        {
            char buffer[128]={0};
            //从管道的另一端读出来,堵塞
            size_t len=read(fd1[0],buffer,128);
            printf("从父进程读到的是%s",buffer);
            if(buffer[0]=='-'&&buffer[1]=='1')
            break;
            int i;
            for(i=0;i<len;i++)
            {
                buffer[i]=toupper(buffer[i]);
            }
            //把从父进程读到的转换成大写字母再写入到父进程
            write(fd2[1],buffer,strlen(buffer));
        }
    }
    
}

int main(void)
{
    int fd1[2];
    int fd2[2];
    pipe(fd1);
    pipe(fd2);
    process(fd1,fd2);
    //父进程
    return 0;
}