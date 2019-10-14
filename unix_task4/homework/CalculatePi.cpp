#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

struct ChildInfo{
	pid_t pid;
	double value;
};
double getpi(uint64_t deep);

int main(int argc, char* argv[]){
	ChildInfo record;

	printf("Calculation Pi started\n");
	
	record.pid = getpid();
	record.value = getpi(1000);

	printf("Calculation Pi ended, Pi: %lf\n", record.value);
	
	//write through offset
	pwrite(atoi(argv[1]), &record, sizeof(struct ChildInfo), (off_t)atoi(argv[2]));

	return 0;
}

double getpi(uint64_t deep){
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
