#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <math.h>


bool file_exist(char *filename){
	struct stat buffer;   
	return (stat (filename, &buffer) == 0);
}

int main(int argc, char * argv[])
{
	char name_socket[100];

	int ret, len = 0;

	if (!file_exist("./main")){
		umask(0);
		if (mknod("./main", S_IFIFO | 0777, 0) < 0){
			perror("mknod");
			exit(2);
		}
		printf("Create main\n");
	}
	int fd_main = open("./main"	, O_WRONLY);
	if (fd_main==-1){
		perror("open");
		return 1;
	}

	printf("Open main\n");

	while (1){
		if (write(fd_main, "h", 1)==-1)
			if (errno==EINTR)
				continue;
			perror("write");
			break;
	}
	return 0;
}