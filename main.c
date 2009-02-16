#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>
#include "bondsh.h"
#include <errno.h>
#include <limits.h>
#include <err.h>
#include <editline/readline.h>


void sighandle_line_clear(int sig) {
	fflush(stdout);
}

void execute_binary(bsh_command_chain_t *chain, char *envp[], const char *path) {
	int i;
	pid_t pid;
	pid = fork();
	if(pid == 0) {
		i = execvP(chain->command, path, chain->args);
		if(i < 0){
			switch(errno) {
				case ENOENT:
					/* attach path */
					errx(errno, "File does not exist: '%s'", chain->command);
				default:
					errx(errno, "Failed to execve(""%s"", argv, envp), errno is %d", chain->command, errno);
			}
		} else {
			print_prompt();
		}
	} else if(pid > 0) {
		wait(NULL);
	}	
}

void execute_chain(bsh_command_chain_t *chain, char *envp[], const char *path) {
	for(;chain != NULL; chain = chain->next) {
		execute_binary(chain, envp, path);
	}
}

int main (int argc, char const *argv[], char *envp[])
{
	setbuf(stdin, NULL);
	/* define signal handlers */
	signal(SIGINT, SIG_IGN);
	signal(SIGINT, sighandle_line_clear);
	
	/* code */
	char line[LINE_BUFFER_MAX];
	bsh_history_chain_t hist;
	history_init(&hist);
	
	/* pick out important info from ENV */
	int i = 0;
	char *path = NULL;
	for(i = 0; envp[i] != NULL; i++) {
		/* PATH= */
		if(*(envp[i]) == 80 && *(envp[i]+1) == 65 && *(envp[i]+2) == 84 && *(envp[i]+3) == 72 && *(envp[i]+4) == 61)
			path = strdup(envp[i]+5);
			assert(path);
	}

	for(;;) {
		bzero(line, sizeof(line));
		bsh_command_chain_t chain;
		chain_init(&chain);
		
		print_prompt();
		
		switch(determine_input_context(&chain)) {
			case CONTEXT_EXECUTE:
				execute_chain(&chain, (char **)envp, path);
				fflush(stdout);

				break;
			case CONTEXT_NOCONTEXT:
				printf("NOCONTEXT caught\n");
				break;
				
			default:
				printf("error!\n");
				exit(-1);
				break;
		}
		//chain_free(&chain);
		history_attach(&hist, &chain);
	}
	
	return 0;
}