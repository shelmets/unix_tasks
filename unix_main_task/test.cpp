#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
    char* buff;
    fgets(buff,10,stdin);
    buff = strstr(buff, "\n");
    
    printf("%s", buff);
}