#include <stdio.h>
#include <fcntl.h>
#include <sys/dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <inttypes.h>
#include <dirent.h>
#define SIZE 256

DIR *D;
struct dirent *de;
struct stat st;
static char* rights = "rwxrwxrwx";

int main(int argc, char * args)
{
	uint16_t right_ind;
	mode_t mode;  
	
	char* opts = "l",copt;	
	int opt;
	D = opendir(".");

	while((de = readdir(D))!=NULL){
		if (de->d_name[0]!='.'){
			lstat(de->d_name,&st);
			mode = st.st_mode;
			S_ISDIR(mode)?printf("d"):printf("-");
			mode &=0777;
			right_ind = 0;
			while( right_ind < 9 )
			{
				mode&256?printf("%c",rights[right_ind]):printf("-");
				mode <<=1;
				right_ind++;
			}
			printf("  %s\n", de->d_name);
		}
	}
	closedir(D);

	return 0;
}
