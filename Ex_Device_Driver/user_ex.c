#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "ex_device.h"

int main()
{
	int buf[1], len;
	int fd,  ret=10;

	printf("start open\n");

	fd = open("/dev/mydev", O_RDWR);
	//FILE *fd = fopen("/dev/mydev", O_RDWR);

	printf("after open\n");
	if(fd < 0) {
		printf("\n can't open device!\n");
		return -1;
	}

	printf("open device\n");

	if(read(fd, buf, sizeof(buf)))
    {printf("read failed\n");}
  else
  {
    printf("\n read num = %d\n", buf[0]);
  }

  buf[0] = 37;
  len = write(fd, buf, sizeof(buf));
  printf("write finish \n");

   // set num to 10
  printf("\n ioctl set to kernel = %d\n", ret);
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


	return 0;
}