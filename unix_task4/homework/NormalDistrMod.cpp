#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdio.h>
#include <math.h>
#define SIZE 7

struct ChildInfo{
	pid_t pid;
	double value;
};

int main(int argc, char * argv[])
{
	pid_t pid;
	int ret, fd, x;

	char** buff = (char**)malloc(3 * sizeof(char*));
	for (int i = 0; i < 3; i++)
		*(buff+i) = (char*)malloc(SIZE*sizeof(char));
	
	char* const xstr =  *buff;
	char* const fdstr = *(buff+1);
	char* const offstr = *(buff+2);


	int * status = (int*)malloc(sizeof(int));
	*status = 0;

	printf("Enter x: ");
	scanf("%i",&x);

	//check on number

	fd = open("./pipe", O_WRONLY | O_CREAT | O_TRUNC, 0664);


	
	sprintf(*buff, "%i",x);
	sprintf(*(buff+1),"%i",fd);
	sprintf(*(buff+2), "%lu", sizeof(ChildInfo));

	pid = fork();

	if (pid==-1){
		perror("First fork");
		exit (EXIT_FAILURE);
	}
	if (!pid){

		printf("First fork started\n");
		char *const argspi[] = {"CalculatePi", fdstr,"0", NULL};

		if (execv("./CalculatePi", argspi)==-1){
			perror("execv");
			exit (EXIT_FAILURE);
		}

		printf("First fork ended\n");

		return 0;
	}

	pid = fork();

	if (pid==-1){
		perror("Second fork");
		exit (EXIT_FAILURE);
	}
	if (!pid){
		printf("Second fork started\n");

		if (execl("./CalculateExp","CalculateExp", xstr ,fdstr,offstr, NULL)==-1){
			perror("execv");
			exit (EXIT_FAILURE);
		}

		printf("Second fork ended\n");
	}

	double total, exp, pi;
	ChildInfo buffinfo;

	printf("Main Process - Waiting\n");
	printf("Main Process - Ended child: %i pid\n", wait(status));
	printf("Main Process - Ended child: %i pid\n", wait(status));

	close(fd);

	fd = open("./pipe",O_RDONLY);

	//read through offset
	if (pread(fd,&buffinfo,sizeof(ChildInfo),0)==(off_t)-1){
		perror("pread");
		exit(EXIT_FAILURE);
	}
	pi = buffinfo.value;

	if (pread(fd,&buffinfo,sizeof(ChildInfo),sizeof(ChildInfo))==(off_t)-1){
		perror("pread");
		exit(EXIT_FAILURE);
	}

	exp  = buffinfo.value;

	total = exp/sqrt(pi*2);

	for (int i = 0; i < 3; i++)
		free(*(buff+i));

	free(buff);
	
	printf("Exp(-%i)/Sqrt(Pi*2): %lf\n",x*x/2,total);

	return 0;
}
