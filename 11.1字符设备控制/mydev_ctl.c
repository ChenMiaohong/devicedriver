#include<sys/types.h>
#include<sys/ioctl.h>
#include<sys/stat.h>
#include<fcntl.h>

#include "mydev.h"





int main()
{	
	int fd;
	fd = open("/dev/memdev0",O_RDWR);
	ioctl(fd,MEM_SET,115200);
	ioctl(fd,MEM_RESTART);
	

	return 0;

}
