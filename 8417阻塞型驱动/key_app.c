#include<stdio.h>
#include<stdlib.h>
#include <errno.h>

int main(int argc,char **argv)
{
	int fd;
	int key_num;
	//1.打开设备
	fd = open("/dev/mykey",0);
	if(fd<0)
		printf("open device fail\n");

	//读取设备
	read(fd,&key_num,4);
	printf("key is %d\n",key_num);


	//关闭设备
	close(fd);
	return 0;
}
