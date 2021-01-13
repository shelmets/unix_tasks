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
	double* s = new double;
	double** x = &s;
	**x = 1;
	printf("%lf\n", **x);
	return 0;
}