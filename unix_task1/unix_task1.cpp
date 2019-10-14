#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#define SIZE 256

int main(void)
{
	int fd, byte; 
	char* buff = new char[SIZE];
	fd = open("./output.txt",O_WRONLY | O_CREAT,0664);
	while ((byte = read(0,buff,SIZE))>1)
	{
		write(fd,buff,byte);
	}
	close(fd);
	fd = open("./output.txt", O_RDONLY);
	while ((byte = read(fd,buff,SIZE))>0)
    {
        write(1,buff,byte);
    }
	return 0;
} 
