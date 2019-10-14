#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

struct ChildInfo{
	pid_t pid;
	double value;
};

double getexp(double x, uint64_t deep);

int main(int argc, char* argv[]){
	ChildInfo record;

	printf("Calculation Exp started\n");
		
	record.pid = getpid();
	record.value = getexp((double)atoi(argv[1]), 1000000);
	
	printf("Calculation Exp ended, Exp: %lf\n", record.value);
	
	//write through offset
	pwrite(atoi(argv[2]), &record, sizeof(struct ChildInfo), (off_t)atoi(argv[3]));

	return 0;
}

double getexp(double x, uint64_t deep){
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
