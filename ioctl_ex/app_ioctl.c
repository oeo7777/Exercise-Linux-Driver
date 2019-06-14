#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "IOC_Test.h"

int main()
{
	char buf[1];
	int fd,  ret=10;

	printf("start open\n");

	fd = open("/dev/mydev", O_RDWR);
	//FILE *fd = fopen("/dev/mydev", O_RDWR);

	printf("after open\n");
	if(fd < 0) {
		printf("\n can't open device!\n");
		return -1;
	}

	read(fd, buf, sizeof(buf));
	//fwrite(buf, sizeof(buf), 1, fp);
   
   // set num to 10
   if (ioctl(fd, SETNUM, &ret)< 0)
   {
       printf("set num failed\n");
       return -1;
   }
   if(ioctl(fd, GETNUM , &ret)< 0)
   {
       printf("get num failed\n");
   }
   printf("get kernel value=%d\n",ret);
   // exchange number ret =atoi(argv[2]);  
   ret = 123;
   printf("send %d to kernel for exchange\n", ret);
   if (ioctl(fd, XNUM, &ret)<0)
   {
       printf("exchange num failed\n");
       return -1;
    }
    else
	   printf("get %d from kernel for exchange \n",ret);
	//fclose(fp);
	return 0;
}

