#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int shell_active = 1; // требуется для команды exit

//  команды оформлены в виде макросов
//  доп. информация http://en.wikipedia.org/wiki/C_preprocessor

#define SHCMD(x) int shcmd_##x (char* cmd, char* params[])
#define SHCMD_EXEC(x) shcmd_##x (params[0], params)
#define IS_CMD(x) strncmp(#x,cmd,strlen(cmd)) == 0

// Встроенные команды
// - pwd, exit

SHCMD(pwd)
{
	printf("%s\n",getenv("PWD"));
	return 0;
}

SHCMD(exit)
{
	shell_active = 0;
	printf("Bye, bye!\n");
	return 0;
}

// выполнение команды с параметрами
void my_exec(char *cmd)
{
	char *params[256]; //параметры команды разделенные пробелами
	char *token;
	int np;

	token = strtok(cmd, " ");
	np = 0;
	while( token && np < 255 )
	{
		params[np++] = token;
		token = strtok(NULL, " ");
	}
	params[np] = NULL;

	// выполнение встроенных команд
	if( IS_CMD(pwd) )
		SHCMD_EXEC(pwd);
	else
	if( IS_CMD(exit) )
		SHCMD_EXEC(exit);
	else
	{ // иначе вызов внешней команды
		execvp(params[0], params);
		perror("exec"); // если возникла ошибка при запуске
	}
}
// рекурсивная функция обработки конвейера
// параметры: строка команды, количество команд в конвейере, текущая (начинается с 0 )
int exec_conv(char *cmds[], int n, int curr)
{
	int fd[2],i;
	if( pipe(fd) < 0 )
	{
		printf("Cannot create pipe\n");
		return 1;
	}

	if( n > 1 && curr < n - 2 )
	{ // first n-2 cmds
		if( fork() == 0 )
		{
			dup2(fd[1], 1);
			close(fd[0]);
			close(fd[1]);
			my_exec(cmds[curr]);
			exit(0);
		}
		if( fork() == 0 )
		{
			dup2(fd[0], 0);
			close(fd[0]);
			close(fd[1]);
			exec_conv(cmds,n,++curr);
			exit(0);
		}
	}
	else
	{ // 2 last cmds or if only 1 cmd
		if( n == 1 && strcmp(cmds[0],"exit") == 0 )
		{ // для случая команды exit обходимся без fork, иначе не сможем завершиться
			close(fd[0]);
			close(fd[1]);
			my_exec(cmds[curr]);
			return 0;
		}
		if( fork() == 0 )
		{
			if( n > 1 ) // if more then 1 cmd
				dup2(fd[1], 1);
			close(fd[0]);
			close(fd[1]);
			my_exec(cmds[curr]);
			exit(0);
		}
		if( n > 1 && fork()==0 )
		{
			dup2(fd[0], 0);
			close(fd[0]);
			close(fd[1]);
			my_exec(cmds[curr+1]);
			exit(0);
		}
	}
	close(fd[0]);
	close(fd[1]);

	for( i = 0 ; i < n ; i ++ ) // ожидание завершения всех дочерних процессов
		wait(0);

	return 0;
}
// главная функция, цикл ввода строк (разбивка конвейера, запуск команды)
int main()
{
	char cmdline[1024];
	char *p, *cmds[256], *token;
	int cmd_cnt;

	while( shell_active )
	{
		printf("[%s]# ",getenv("PWD"));
		fflush(stdout);

		fgets(cmdline,1024,stdin);
		if( (p = strstr(cmdline,"\n")) != NULL ) p[0] = 0;

		token = strtok(cmdline, "|");
		for(cmd_cnt = 0; token && cmd_cnt < 256; cmd_cnt++ )
		{
			cmds[cmd_cnt] = strdup(token);
			token = strtok(NULL, "|");
		}
		cmds[cmd_cnt] = NULL;

		if( cmd_cnt > 0 )
		{
			exec_conv(cmds,cmd_cnt,0);
		}
	}

	return 0;
}
