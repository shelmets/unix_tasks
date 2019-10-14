#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <math.h>

struct ChildInfo{
	pid_t pid;
	double value;
	char object;
};

double getExp(double x, uint64_t deep){
	double sum = 1;
	double elem = 1;
	if (deep>0){
		for (uint64_t i = 1; i <= deep;i++){
			elem*=double((x*x)/2)/double(i);
			sum+=elem;
		}
		return 1/sum;
	}
	else {
		return -1;
	}
}

double getPi(uint64_t deep){
	double sum = 1;
	if (deep>0){
		for (uint64_t i = 1; i <= 2*deep;i++){
			sum+=(double)(i%2==0?1:-1)/(double)(2*i+1);
		}
		return sum*4;
	}
	else
		return -1;	
}

int main(int argc, char * argv[])
{
	int fd;
	double x;

	int * status = new int;
	*status = 0;

	printf("Enter x: ");
	scanf("%lf",&x);

	fd = open("./pipe", O_WRONLY | O_CREAT | O_TRUNC ,0664);

	pid_t pid1, pid2;
	pid1 = fork();

	if (pid1==0){
		printf("First fork started\n");
		printf("Calculation Exp started\n");
		
		struct ChildInfo record1;
		record1.pid = pid1;
		record1.value = getExp(x, 1000000);
		record1.object = 'e';
		
		printf("Calculation Exp ended, Exp: %lf\n", record1.value);
		
		write(fd, &record1, sizeof(struct ChildInfo));

		printf("First fork ended\n");
	}
	else if (pid1 > 0){
		pid2 = fork();

		if (pid2==0){
			printf("Second fork started\n");
			printf("Calculation Pi started\n");
			
			struct ChildInfo record2;
			record2.pid = pid1;
			record2.value = getPi(1000);
			record2.object = 'p';

			printf("Calculation Pi ended, Pi: %lf\n", record2.value);
			
			write(fd, &record2, sizeof(struct ChildInfo));

			printf("Second fork ended\n");
		}
		else if (pid2>0){
			double total, exp, pi;
			ChildInfo buff;
			size_t byte;

			printf("Main Process - Waiting\n");
			printf("Main Process - Ended child: %i pid\n", wait(status));
			printf("Main Process - Ended child: %i pid\n", wait(status));

			close(fd);

			fd = open("./pipe",O_RDONLY);

			
			while ((byte = read(fd,&buff,sizeof(ChildInfo)))>1){
				switch(buff.object){
					case 'p':
						pi  = buff.value;
						break;
					case 'e':
						exp = buff.value;
						break;
					default:
						printf("Error, buff.object is neither \'p\' nor \'e\'\n");
						return 1;
				}
			}
			total = exp/sqrt(pi*2);

			printf("Exp(-%lf)/Sqrt(Pi*2): %lf\n",x*x/2,total);
		}
		else{
			printf("Second fork failed!\n");
			return 1;
		}
	}
	else{
		printf("First fork failed!\n");
		return 1;
	}
	return 0;
}