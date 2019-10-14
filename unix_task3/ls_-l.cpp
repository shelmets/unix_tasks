#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <sys/dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <uuid/uuid.h>
#include <inttypes.h>
#include <grp.h>
#include <dirent.h>
#define SIZE 256

DIR *D;
bool LongFormat = false;
const char* monthArr[12] = {"Jan","Feb", "Mar","Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

char* getRight(struct stat st)
{
	uint16_t right_ind;
	uint16_t i = 0;
	mode_t mode;
	const char* rights = "rwxrwxrwx";
	char* buff = new char[SIZE];
	mode = st.st_mode;
	if (S_ISDIR(mode))
		buff[i++] = 'd';
	else
		buff[i++] = '-';
	mode &=0777;
	right_ind = 0;
	while( right_ind < 9 )
	{
		if (mode&256)
			buff[i++] = rights[right_ind];
		else
			buff[i++] = '-';
		right_ind++;
		mode <<=1;
	}
	return buff;
}


int main(int argc, char * argv[])
{
	char* opts = "l",copt;	
	int opt;
	while ((opt = getopt(argc,argv,opts))!=-1){
		switch(opt){
			case 'l':
				LongFormat = true;
		}
	}

	D = opendir(".");

	struct dirent *de;
	struct stat st;
	struct tm* mtime;

	while((de = readdir(D))!=NULL){
		if (de->d_name[0]!='.'){
			if (LongFormat){
				mtime = localtime(&st.st_mtimespec.tv_sec);
				lstat(de->d_name,&st);
				printf("%s || ", getRight(st));
				printf("%u || ", st.st_nlink);
				printf("%s || ", getpwuid(st.st_uid)->pw_name);
				printf("%s || ", getgrgid(st.st_gid)->gr_name);
				printf("%lld || ", st.st_size);
				printf("%u %s %u:%u || ", mtime->tm_mday, *(monthArr + mtime->tm_mon), mtime->tm_hour, mtime->tm_min);
			}
			printf("%s\n", de->d_name);
		}
	}
	closedir(D);

	return 0;
}
