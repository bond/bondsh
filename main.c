#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "bondsh.h"
#include <errno.h>


void sighandle_line_clear(int sig) {
	fflush(stdout);
}

void execute_binary(const char *command, char *argv[], char *envp[]) {
	int i;
	pid_t pid;

	pid = fork();
	if(pid == 0) {
		i = execve(command, argv, envp);	
		if(i < 0){
			printf("failed to execve(""%s"", argv, envp),errno is %d\n", command, errno);
			exit(0);
		} else {
			print_prompt();
		}
	} else if(pid > 0) {
		wait(NULL);
	}
}

int main (int argc, char const *argv[], char const *envp[])
{
	/* define signal handlers */
	signal(SIGINT, SIG_IGN);
	signal(SIGINT, sighandle_line_clear);
	
	/* code */
	char line[LINE_BUFFER_MAX];
	
	int i;
	bsh_command_chain_t chain;
	
	for(;;) {
		bzero(line, sizeof(line));
		
		chain_init(&chain);
		
		print_prompt();
		
		switch(determine_input_context(&chain)) {
			case CONTEXT_EXECUTE:
			//	execute_binary(voidp_input, (char**)argv, (char**)envp);

				break;
			case CONTEXT_NOCONTEXT:
				execute_binary(chain.command, (char **)&(chain.args), (char **)envp);
				fflush(stdout);
				break;
				
			default:
				printf("error!\n");
				exit(-1);
				break;
		}
		chain_free(chain);
	}
	
	return 0;
}