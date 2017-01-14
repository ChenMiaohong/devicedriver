#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/ioctl.h>
#include "led.h"
#include<stdio.h>

int main(int argc,char *argv[])
{
	int fd;
	int cmd;
	if(argc<2)
	{
		printf("please input second para\n");
		return 0;

	}
	cmd = atoi(argv[1]);
	fd = open("/dev/myled0",O_RDWR);
	if(cmd ==1)
	ioctl(fd,LED_ON);
	if(cmd==0)
	ioctl(fd,LED_OFF);

	return 0;

}
