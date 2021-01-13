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
#include <time.h>

void alarm_handler(int signum){
	printf("Exit from %d\n", getpid());
	exit(0);
}
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
	int fd_main = open("./main"	, O_RDONLY | O_NONBLOCK);
	if (fd_main==-1){
		printf("Error");
		perror("open");
		return 1;
	}

	printf("Open main\n");

	while (1){
		while((ret = read(fd_main, name_socket + (len++), 1))!=0){
			if (ret == -1){
				if (errno == EINTR || errno == EAGAIN){
					--len;
					continue;
				}
				perror("read");
				len = 0;
				break;
			}
			if (*(name_socket + len - 1)=='\n'){
				*(name_socket + len - 1)='\0';

				printf("Get %s\n", name_socket);
				pid_t pid;
				pid = fork();

				if (pid==0){
					usleep(2000000);
					int socket = open(name_socket, O_WRONLY);
					if (socket==-1){
						perror("open");
						return 0;
					}
					printf("Open %s\n", name_socket);
					char * ch = new char[2];
					ch[1] = '\0';
					printf("Writing\n");

					signal(SIGALRM, alarm_handler);
					alarm(30);

					while (1){
						srand(time(NULL));
						ch[0] = rand() % 256 + '0';
						setpriority(PRIO_PROCESS, pid, rand()%10);
						if (write(socket, ch, 1)==-1){
							perror("write");
							break;
						}
					}
					return 0;
				}
				break;
				}
			}
		len = 0;
		sleep(1);
		} 

	return 0;
}