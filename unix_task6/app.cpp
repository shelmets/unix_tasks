#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>

bool file_exist(char *filename){
	struct stat buffer;   
	return (stat (filename, &buffer) == 0);
}

int main(int argc, char * argv[])
{
	int fd_main = open("./main", O_WRONLY);

	if (fd_main==-1){
		perror("open");
		return 1;
	}
	
	printf("Open file main\n");

	pid_t pid = getpid();

	printf("Pid: %d\n", pid);

	char name_socket[100];
	char byte;

	sprintf(name_socket, "app%i", pid);

	if (!file_exist(name_socket)){
		umask(0);
		if (mknod(name_socket, S_IFIFO | 0777, 0) < 0){
			perror("mknod");
			exit(2);
		}
		printf("Create socket %s\n", name_socket);
	}

	int len = strlen(name_socket);
	name_socket[len] = '\n';
	name_socket[len+1] = '\0';


	if (write(fd_main, name_socket, len + 1) == -1){
		perror("write");
		exit(2);
	}
	printf("Write name_socket\n");

	close(fd_main);
	name_socket[len] = '\0';

	int fd_app = open(name_socket, O_RDONLY);

	if (fd_app==-1){
		perror("open");
		exit(2);
	}

	printf("Open socket %s\n", name_socket);

	printf("Wait\n");

	int ret, count = 0;

	while ((ret = read(fd_app, &byte, 1))!=0){
		if (ret==-1){
			if (errno == EINTR || errno == EAGAIN)
				continue;
			perror("read");
			break;
		}
		count++;
	}

	printf("%i\n", count);

	close(fd_app);
	printf("Close %s\n", name_socket);

	if (unlink(name_socket)==-1){
		perror("unlink");
		exit(2);
	}
	
	return 0;
}