//内存映射方式实现文件排序
//文件数据用随机数生成，将生成好的文件映射到内存，对其进行排序，查看结果
//提示：生成100万个数据，用srand(time(NULL))生成种子，
//然后用rand()生成一个随机数，随机数的范围是0-1,的32次幂-1
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#define N 1000000
//生成随机数并写入文件
void create_file(const char *a)
{
    int fd=open(a,O_CREAT|O_EXCL|O_RDWR|O_APPEND,0666);
    if(fd==-1)
    {
        if(errno==EEXIST)
        {
            fd=open(a,O_RDWR|O_TRUNC);
        }
        else
        {
            perror("open");
            exit(-1);
        }
    }
    //创建种子
    srand(time(NULL));
    int buffer[N]={0};
    int i;
    //生成随机数
    for(i=0;i<N;i++)
    {
        buffer[i]=rand()%N;
    }
    write(fd,buffer,sizeof(buffer));
    close(fd);
}
//内存映射返回addr
void *fileaddr(const char *a)
{
    int fd=open(a,O_RDWR,0666);
    if(fd==-1)
    {
        if(errno==EEXIST)
        {
            fd=open(a,O_RDWR|O_APPEND);
        }
        else
        {
            perror("open");
            exit(-1);
        }
    }
    int buffer[N]={0};
    char *addr=mmap(0,N*4,PROT_WRITE|PROT_READ,MAP_SHARED,fd,0);
    if(addr==MAP_FAILED)
    {
        perror("mmap");
        return NULL;
    }
    close(fd);
    return addr;
}
//快速排序
void quick_sort(int *a,int left,int right)
{
	//保存两个参数
	int i=left;
	int j=right;
	//拿第一个元素当做比较值
	int compare_number=a[left];

	while(i<j)
	{
		//从右边开始寻找第一个小于比较值的数
		while(a[j]>=compare_number&&i<j)
		{
			j--;
		}

		//找到后，则放到左边
		if(i<j)//控制i和j不越界
		{
			a[i]=a[j];//第一次相当于a[0]=30
			i++;
			//第一次赋值后的结果是
		//30,12,58,87,64,41,10,78,96,65,66,30
		}		

		//从左边开始寻找第一个大于比较值的数
		while(a[i]<=compare_number&&i<j)
		{
			i++;
		}
		//左边的值放到右边
		if(i<j)//下标越界控制
		{
			a[j]=a[i];
			j--;
		}		
	}
	//循环完后，将比较值放入a[i]
	a[i]=compare_number;

	//如果切分后的数组不是一个元素，则继续处理
	if(left<right)
	{
		quick_sort(a,left,i-1);
		quick_sort(a,i+1,right);	
	}

}
int main(int argc,const char *argv[])
{
    if(argc<2)
    {
        printf("使用方法: %s 文件名\n",argv[0]);
        exit(-1);
    }
    create_file(argv[1]);
    int *addr=fileaddr(argv[1]);
    quick_sort(addr,0,N-1);
    int i;
    for(i=0;i<N;i++)
    {
        printf("%d\t",addr[i]);
    }
    printf("\n");
    munmap(addr,N*4);
    return 0;
}