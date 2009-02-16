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
		// chain->fds[0] = dup2(0, STDIN_FILENO);
		// chain->fds[1] = dup2(0, STDOUT_FILENO);
		i = execvP(chain->command, path, chain->args);
		if(i < 0){
			switch(errno) {
				case ENOENT:
					/* attach path */
					errx(errno, "Could not find program: '%s' in PATH", chain->command);
				default:
					errx(errno, "Failed to execve(""%s"", argv, envp), errno is %d", chain->command, errno);
			}
		} else {
			print_prompt();
		}
	} else if(pid > 0) {
		//printf("filedes: %i\n", chain->fds[1]);
		wait(NULL);
	}	
}

void execute_chain(bsh_command_chain_t *chain, char *envp[], const char *path) {
	for(;chain != NULL; chain = chain->next) {
		execute_binary(chain, envp, path);
		//printf("executing command: %s\n", chain->command);
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
		bsh_command_chain_t *chain = chain_init();
		bsh_command_chain_t *next = chain;
		
		print_prompt();
		
MORE_INPUT:
		switch(determine_input_context(next)) {
			case CHAIN_WANT_PROCESS_PIPE:
				chain->next = chain_init();
				next = chain->next;
				goto MORE_INPUT;
				break;

			case CONTEXT_EXECUTE:
				execute_chain(chain, (char **)envp, path);
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
		chain_free(chain);
		// history_attach(&hist, &chain);
	}
	
	return 0;
}