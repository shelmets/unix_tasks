#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>


struct ChildInfo{
	pid_t pid;
	double value;
};

double getExp(double x, int deep){
	double sum = 1;
	double elem = 1;
	for (int i = 1; i <= deep;i++){
		elem*=double((x*x)/2)/double(i);
		printf("%lf\n",elem);
		sum+=elem;
	}
	return 1/sum;
}

double getPi(int deep){
	double sum = 1;
	if (deep>0){
		for (int i = 1; i <= 2*deep;i++){
			sum+=(double)(i%2==0?1:-1)/(double)(2*i+1);
		}
		return sum*4;
	}
	else
		return -1;	
}

int main(int argc, char * argv[])
{
	int fd, pid1;
	double x;
	double total;
	size_t byte;

	int * status = new int;

	*status = 0;

	printf("Enter x: ");
	scanf("%lf",&x);

	fd = open("./pipe", O_WRONLY | O_CREAT | O_TRUNC ,0664);

	printf("Calculation Exp started\n");
	ChildInfo record1;

	ChildInfo record2;
	
	record1.pid = 0;
	record1.value = getExp(x, 10);
	printf("getExp: %lf\n",record1.value);
	pwrite(fd, &record1, sizeof(ChildInfo), (off_t)0);
	
	record2.pid = 1;
	record2.value = getPi(1000);
	printf("getPi: %lf\n",record2.value);
	pwrite(fd, &record2, sizeof(ChildInfo), (off_t)sizeof(ChildInfo));

	close(fd);	

	fd = open("./pipe", O_RDONLY);

	while ((byte = read(fd,&record2,sizeof(ChildInfo)))>1)
	{
		printf("pid : %i\n", record2.pid);
		printf("value: %lf\n", record2.value);
	}
	return 0;
}