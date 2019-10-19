#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <math.h>

struct message
{
	double x;
	uint64_t deep;
};

void* getExp(void *mess){
	double sum = 1;
	double elem = 1;
	double deep = ((message*)mess)->deep;
	double x = ((message*)mess)->x;
	if (deep>0){
		for (uint64_t i = 1; i <= deep;i++){
			elem*=double((x*x)/2)/double(i);
			sum+=elem;
		}
		((message*)mess)->x = 1/sum;
		return mess;
	}
	else {
		return NULL;
	}
}

void* getPi(void *mess){
	double sum = 1;
	double deep = ((message*)mess)->deep;
	if (deep>0){
		for (uint64_t i = 1; i <= 2*deep;i++){
			sum+=(double)(i%2==0?1:-1)/(double)(2*i+1);
		}
		((message*)mess)->x = sum*4;
		return mess;
	}
	else
		return NULL;	
}

int main(int argc, char * argv[])
{
	double total, exp, pi;
	pthread_t thing1, thing2;

	struct message* messexp = (struct message*)malloc(sizeof(struct message));
	messexp->deep = 100000;

	struct message* messpi = (struct message*)malloc(sizeof(struct message));
	messpi->x = 0;
	messpi->deep = 100000;


	printf("Enter x: ");
	scanf("%lf", &(messexp->x));

	
	pthread_create(&thing1, NULL, getExp, (void*)messexp);
	pthread_create(&thing2, NULL, getPi, (void*)messpi);

	pthread_join(thing1, NULL);
	pthread_join(thing2, NULL);

	exp = messexp->x;
	printf("%lf\n", exp);
	pi = messpi->x;
	printf("%lf\n", pi);
	total = exp/sqrt(pi*2);

	printf("Total: %lf\n",total);

	return 0;
}