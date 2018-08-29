#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#define BUF_SIZE 20
int fd,retval;
char buffer[BUF_SIZE];

int main()
{
	int i=0;
	short int result=0;
	float temp=0;
	printf("User app\nopening the device file\n");
	fd=open("/dev/MPU6050",O_RDWR,0777);
	if(fd<0)
	{
		printf("error in file open\n");
		exit (-1);
	}
	printf("Reading device file data\n");
	
	retval=read(fd,buffer,strlen(buffer));
	if(retval<0)
	{
		printf("fail in read\n");
		exit (-1);
	}
	while(i<15)
	{
		result=((buffer[i]<<8) |  buffer[i+1]);
		if(i==0 || i<=5)
		{
			printf("Accelerometer data= %d \n",result);
		}
		else if(i==6 || i<=7)
		{
			temp = (result/340.00 + 36.53);
			printf("Temperature data= %.2f \n",temp);
		}
		else
		{
			printf("Gyroscope data= %d \n",result);
		}

		i=i+2;
	}

	close(fd);
	return 0;
}

